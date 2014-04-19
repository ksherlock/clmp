/*
 * Command Line MIDI Player
 *
 * by Kelvin W Sherlock
 *
 *
 * Sorry, BeOS only, losers!!
 *
 *
 */

// Posix header files
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// BeOS header files
#include <MidiSynthFile.h>
#include <Entry.h>


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif



/*
 * Prints the name & version ofthe program to stdout.
 *
 *
 */
void PrintVersion(void)
{
	fprintf(stdout, "clmp - Command Line MIDI Player v 1.0\n"
		"copyright 1999 by Kelvin W Sherlock\n");
}

/*
 * Prints help to stdout
 *
 *
 */
void PrintHelp(void)
{
	fprintf(stdout, "usage: clmp [flags] midi files..."
		"flags:\n"
		"-v\t--verbose    Be verbose\n"
		"-s\t--silent     Be silent\n"
		"  \t--version    Show version information and exit\n"
		"-h\t--help       Show usage information\n"
		"-r\t--repeat     Repeat song\n"
		"-r\t--randomize  Randomize songs\n"
		"\n"
		"-R\t--reverb     Set reverb (1-6)\n"
		"-V\t--volume     Adjust synth volume\n"
		"-S\t--sample     Adjust sample volume\n"
		);
}


int main(int argc, char **argv)
{

int optchar;
int verbose = FALSE;
int silent = FALSE;
int randomize = FALSE;
int number_files = 0;
int reverb = 4;
float synth_volume = 1.0;
float sample_volume = 1.0;
status_t error;

BSynth Synth;
BMidiSynthFile mFile;
entry_ref file;

/*
 * data for long getopt
 *
 *
 */
static struct option long_options[] = 
{
	{
		"verbose",	// name
		no_argument,	// has flag?
		NULL,		// return val
		'v'		// value to return
	},
	{
		"silent",	// name
		no_argument,	// has flag?
		NULL,		// return val
		's'		// value to return
	},
	{
		"version",
		no_argument,
		NULL,
		'V'
	},
	{
		"help",
		no_argument,
		NULL,
		'h'
	},
	{
		"repeat",
		no_argument,
		NULL,
		'r'
	},
	{
		"randomize",
		no_argument,
		NULL,
		'r'
	},
	{
		"reverb",
		required_argument,	//requires argument
		NULL,
		'R'
	},
	{
		"volume",
		required_argument,
		NULL,
		'V'
	},
	{
		"sample",
		required_argument,
		NULL,
		'S'
	},
	{ NULL, 0, NULL, 0}		//final element MUST be 0-filled
};
	
	

	
	


/*
 * First, parse the arguments
 *
 *
 *
 */


	while ((optchar = getopt_long(argc, argv, "vshrR:V:S:", long_options, NULL)) 
			!= EOF)
	{
		switch(optchar)
		{
		case 'v':	//verbose
			if (silent)
				fprintf(stderr, "-v and -s are mutually "
					"exclusive!\n");
			verbose = TRUE;
			silent = FALSE;
			break;

		case 's':	//silent
			if (verbose)
				fprintf(stderr, "-s and -v are mutually "
					"exclusive!\n");
			silent = TRUE;
			verbose = FALSE;
			break;

		case 'V':		// adjust volume
			synth_volume = atof(optarg);
			break;
		case 'S':		// adjust volume
			sample_volume = atof(optarg);
			break;

		case 'h':
			PrintHelp();
			exit(0);
			break;

		case 'r':		// randomize/repeat
			randomize = TRUE;
			break;
	
		case 'R':		//reverb
			// set reverb by optarg
			reverb = atoi(optarg);
			// prevent illegal values
			if (reverb < 1) reverb = 1;
			if (reverb > 6) reverb = 6;
			break;

		case '?':	//unsupported flag!!
			if (!silent) PrintHelp();
			exit(1);
			break;

		}
	}

	argc -= optind;
	argv += optind;

	number_files = argc;

	if (number_files == 0 && !silent)
	{
		PrintHelp();
		exit(1);
	}

	if (verbose)
		fprintf(stdout, "Loading synth samples\n");
	Synth.LoadSynthData(B_BIG_SYNTH);

	Synth.SetReverb((reverb_mode)reverb);
	Synth.SetSynthVolume(synth_volume);
	Synth.SetSampleVolume(sample_volume);
	if (randomize)
	{
		srand(time(NULL));

		while (number_files)
		{
		int i;
			i = number_files > 1 ? rand()  % number_files : 0;


			if (verbose)
				fprintf(stdout, "File %s\n", argv[i]);

			//get refs on it
			error = get_ref_for_path(argv[i], &file);
			if (error)
			{
				if (!silent) 
					fprintf(stderr, 
						"unable to get refs on file %s\n",
						argv[i]);
			}
			else
			{
				error = mFile.LoadFile(&file);
				if (error & !silent)
				{
					fprintf(stderr, "Unable to load %s\n", 
						argv[i]);
				}
			}

			/*
			 * error is TRUE if the file couldn't be loaded
			 * or if it couldn't be referenced
			 */
			if (error)
			{
				// move the argvs
				number_files--;
				argv[i] = NULL;
				if (i != number_files)
				{
					memmove(argv+i, argv+i+1, 
						(number_files-i) * sizeof(char *));
				}
			}
			else
			{
				mFile.Start();
				while (!mFile.IsFinished())
					snooze(100000);

				mFile.Fade();
			}
		}
		

	}
	else
	{
	int i;
		for (i =0 ; i < number_files; i++)
		{
			if (verbose)
				fprintf(stdout, "File %s\n", argv[i]);

			//get refs on it
			error = get_ref_for_path(argv[i], &file);
			if (error)
			{
				if (!silent)
					fprintf(stderr, 
						"unable to get refs on file %s\n",
						argv[1]);
			}
			else
			{
				error = mFile.LoadFile(&file);
				if (error & !silent)
				{
					fprintf(stderr, "Unable to load %s\n", 
						argv[i]);
				}
			}

			if (!error)
			{
				mFile.Start();
				while (!mFile.IsFinished())
					snooze(100000);

				mFile.Fade();
			}
		}
	}

	return 0;

}


