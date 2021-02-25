#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include <pigpio.h>

/*
freq_count_2.c
2014-08-21
Public Domain

gcc -o freq_count_2 freq_count_2.c -lpigpio -lpthread
$ sudo ./freq_count_2  4 7 8

This program uses the gpioSetGetSamplesFunc function to request
a callback once a millisecond for all accumulated gpio changes
in that millisecond.

This tends to be more efficient then calling a callback for each
gpio change during the millisecond.

EXAMPLES

Monitor gpio 4 (default settings)
sudo ./freq_count_2  4

Monitor gpios 4 and 8 (default settings)
sudo ./freq_count_2  4 8

Monitor gpios 4 and 8, sample rate 2 microseconds
sudo ./freq_count_2  4 8 -s2

Monitor gpios 7 and 8, sample rate 4 microseconds, report every second
sudo ./freq_count_2  7 8 -s4 -r10

Monitor gpios 4,7, 8, 9, 10, 23 24, report five times a second
sudo ./freq_count_2  4 7 8 9 10 23 24 -r2

Monitor gpios 4, 7, 8, and 9, report once a second, sample rate 1us,
generate 2us edges (4us square wave, 250000 highs per second).
sudo ./freq_count_2  4 7 8 9 -r 10 -s 1 -p 2

*/

/*
times with minimal_clk on gpio 4 and 6
sudo ./freq2 4 6 -r10
13% 100k 100k

*/

#define MAX_GPIOS 32

#define OPT_P_MIN 1
#define OPT_P_MAX 1000
#define OPT_P_DEF 20

#define OPT_R_MIN 1
#define OPT_R_MAX 10
#define OPT_R_DEF 5

#define OPT_S_MIN 1
#define OPT_S_MAX 10
#define OPT_S_DEF 5

static volatile int g_pulse_count[MAX_GPIOS];
static volatile int g_update_counts;
static uint32_t g_mask;

static int g_num_gpios;
static int g_gpio[MAX_GPIOS];

static int g_opt_p = OPT_P_DEF;
static int g_opt_r = OPT_R_DEF;
static int g_opt_s = OPT_S_DEF;
static int g_opt_t = 0;

static uint32_t reportInterval = OPT_R_DEF * 100000;

void usage()
{
   fprintf
   (stderr,
      "\n" \
      "Usage: sudo ./freq_count_2 gpio ... [OPTION] ...\n" \
      "   -p value, sets pulses every p micros, %d-%d, TESTING only\n" \
      "   -r value, sets refresh period in deciseconds, %d-%d, default %d\n" \
      "   -s value, sets sampling rate in micros, %d-%d, default %d\n" \
      "\nEXAMPLE\n" \
      "sudo ./freq_count_2 4 7 -r2 -s2\n" \
      "Monitor gpios 4 and 7.  Refresh every 0.2 seconds.  Sample rate 2 micros.\n" \
      "\n",
      OPT_P_MIN, OPT_P_MAX,
      OPT_R_MIN, OPT_R_MAX, OPT_R_DEF,
      OPT_S_MIN, OPT_S_MAX, OPT_S_DEF
   );
}

void fatal(int show_usage, char *fmt, ...)
{
   char buf[128];
   va_list ap;

   va_start(ap, fmt);
   vsnprintf(buf, sizeof(buf), fmt, ap);
   va_end(ap);

   fprintf(stderr, "%s\n", buf);

   if (show_usage) usage();

   fflush(stderr);

   exit(EXIT_FAILURE);
}

static int initOpts(int argc, char *argv[])
{
   int i, opt;

   while ((opt = getopt(argc, argv, "p:r:s:")) != -1)
   {
      i = -1;

      switch (opt)
      {
         case 'p':
            i = atoi(optarg);
            if ((i >= OPT_P_MIN) && (i <= OPT_P_MAX))
               g_opt_p = i;
            else fatal(1, "invalid -p option (%d)", i);
            g_opt_t = 1;
            break;

         case 'r':
            i = atoi(optarg);
            if ((i >= OPT_R_MIN) && (i <= OPT_R_MAX))
               g_opt_r = i;
            else fatal(1, "invalid -r option (%d)", i);
            break;

         case 's':
            i = atoi(optarg);
            if ((i >= OPT_S_MIN) && (i <= OPT_S_MAX))
               g_opt_s = i;
            else fatal(1, "invalid -s option (%d)", i);
            break;

        default: /* '?' */
           usage();
           exit(-1);
        }
    }
   return optind;
}

void samples(const gpioSample_t *samples, int numSamples)
{
   static int inited = 0;

   static uint32_t lastLevel;
   static uint32_t lastReportTick;
   static int count[MAX_GPIOS];

   uint32_t high, level;
   int i, g, s, tickDiff;

   if (!inited)
   {
      inited = 1;

      lastLevel = samples[s].level;
      lastReportTick = samples[0].tick;

      for (i=0; i<g_num_gpios; i++)
      {
         count[i] = 0;
      }
   }

   for (s=0; s<numSamples; s++)
   {
      tickDiff = samples[s].tick - lastReportTick;
      if (tickDiff >= reportInterval)
      {
         lastReportTick = samples[s].tick;

         if (g_update_counts)
         {
            for (i=0; i<g_num_gpios; i++) g_pulse_count[i] = count[i];
            g_update_counts = 0;
         }

         for (i=0; i<g_num_gpios; i++) count[i] = 0;
      }

      level = samples[s].level;
      high = ((lastLevel ^ level) & g_mask) & level;
      lastLevel = level;

      /* only interested in low to high */
      if (high)
      {
         for (g=0; g<g_num_gpios; g++)
         {
            if (high & (1<<g_gpio[g])) count[g]++;
         }
      }
   }
}

int main(int argc, char *argv[])
{
   int i, rest, g, wave_id, mode;
   gpioPulse_t pulse[2];
   int count[MAX_GPIOS];
   float perSec;

   /* command line parameters */

   rest = initOpts(argc, argv);

   reportInterval = g_opt_r * 100000;

   perSec = 10.0 / g_opt_r;

   /* get the gpios to monitor */

   g_num_gpios = 0;

   for (i=rest; i<argc; i++)
   {
      g = atoi(argv[i]);
      if ((g>=0) && (g<32))
      {
         g_gpio[g_num_gpios++] = g;
         g_mask |= (1<<g);
      }
      else fatal(1, "%d is not a valid g_gpio number\n", g);
   }

   if (!g_num_gpios) fatal(1, "At least one gpio must be specified");

   printf("Monitoring gpios");
   for (i=0; i<g_num_gpios; i++) printf(" %d", g_gpio[i]);
   printf("\nSample rate %d micros, refresh rate %d deciseconds\n",
      g_opt_s, g_opt_r);

   gpioCfgClock(g_opt_s, 1, 1);

   if (gpioInitialise()<0) return 1;

   gpioWaveClear();

   pulse[0].gpioOn  = g_mask;
   pulse[0].gpioOff = 0;
   pulse[0].usDelay = g_opt_p;

   pulse[1].gpioOn  = 0;
   pulse[1].gpioOff = g_mask;
   pulse[1].usDelay = g_opt_p;

   gpioWaveAddGeneric(2, pulse);

   wave_id = gpioWaveCreate();

   /* monitor g_gpio level changes */

   gpioSetGetSamplesFunc(samples, g_mask);

   mode = PI_INPUT;

   if (g_opt_t)
   {
      gpioWaveTxSend(wave_id, PI_WAVE_MODE_REPEAT);
      mode = PI_OUTPUT;
   }

   for (i=0; i<g_num_gpios; i++) gpioSetMode(g_gpio[i], mode);

   while (1)
   {
      usleep(50000);
      if (!g_update_counts)
      {
         for (i=0; i<g_num_gpios; i++)
            printf(" %2d=%6d", g_gpio[i], (int)(perSec * g_pulse_count[i]));
         printf("\n");
         g_update_counts = 1;
      }
   }

   gpioTerminate();
}

