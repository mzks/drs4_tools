/********************************************************************\

  Name:         drscmd.cpp
  Created by:   Keita Mizukoshi

\********************************************************************/

#include <math.h>

#ifdef _MSC_VER

#include <windows.h>

#elif defined(OS_LINUX)

#define O_BINARY 0

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DIR_SEPARATOR '/'

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "strlcpy.h"
#include "DRS.h"
#include "cmdline.h"
#include <sys/time.h>
/*------------------------------------------------------------------*/

int main(int argc, char** argv)
{

    cmdline::parser pars;
    pars.add<std::string>   ("datadir",     'l', "Data directory",      false, "/Users/mzks/drs4/data/");
    pars.add<std::string>   ("filename",    'f', "Output file name",    false, "test");
    pars.add<unsigned long int>("num_of_evt",'n', "Number of events",   false, 10);
    pars.add<int>           ("trigger_ch",  'c', "Trigger channel (1--4) 0 is ext.",false, 1);
    pars.add<bool>          ("edge_neg",    'e', "Trigger type (negative or positive)", false, true);
    pars.add<double>         ("trigger",     't', "Trigger level (V)",  false, 0.0);
    pars.add("neg_trig", 'u', "Negative trigger level");
    pars.add<int>           ("delay",       'd', "Trigger delay (ns)",  false, 0.0);
    pars.add<double>         ("sampling",    's', "Sampling Frequency (GHz)",  false, 1.0);
    pars.add<double>         ("center",      'i', "Input range Center (V)",  false, 0.0);
    pars.add("reset", 'r', "DRS4 Reset without data taking");
    pars.parse_check(argc, argv);

    int i, nBoards;
    DRS *drs;
    DRSBoard *b;
    float time_array[8][1024];
    float wave_array[8][1024];
    FILE  *f;
    FILE  *f_data;

    /* do initial scan */
    drs = new DRS();

    /* show any found board(s) */
    for (i=0 ; i<drs->GetNumberOfBoards() ; i++) {
        b = drs->GetBoard(i);
        printf("Found DRS4 evaluation board, serial #%d, firmware revision %d\n", 
                b->GetBoardSerialNumber(), b->GetFirmwareVersion());
    }

    /* exit if no board found */
    nBoards = drs->GetNumberOfBoards();
    if (nBoards == 0) {
        printf("No DRS4 evaluation board found\n");
        return 0;
    }

    /* continue working with first board only */
    b = drs->GetBoard(0);

    /* initialize board */
    b->Init();

    /* set sampling frequency */
    b->SetFrequency(pars.get<double>("sampling"), true);

    /* enable transparent mode needed for analog trigger */
    b->SetTranspMode(1);

    /* set input range to -0.5V ... +0.5V */
	double center = pars.get<double>("center");
	if( center < 0.0 || 0.5 < center){
		printf("Input range center should be set 0 -- 0.5 V\n");
		return 0;
	}
    b->SetInputRange(center);

	double trigger;
	if(pars.get<int>("trigger_ch") == 0){
		// external trigger
		std::cout << "Use External TTL trigger " << std::endl;
		b->EnableTrigger(1, 0);           // enable hardware trigger
		b->SetTriggerSource(1<<4);        // set external trigger as source

	} else {
		// internal trigger 
			b->EnableTrigger(1, 0);       // enable hardware trigger
			b->SetTriggerSource(1 << (pars.get<int>("trigger_ch")-1));

			if(pars.exist("neg_trig")){
				trigger = pars.get<double>("trigger") * -1.;
			}else{
				trigger = pars.get<double>("trigger");
			}
			b->SetTriggerLevel(trigger);            // V
			b->SetTriggerPolarity(pars.get<bool>("edge_neg")); 
	}

	/* use following lines to set individual trigger elvels */
	//b->SetIndividualTriggerLevel(1, 0.1);
    //b->SetIndividualTriggerLevel(2, 0.2);
    //b->SetIndividualTriggerLevel(3, 0.3);
    //b->SetIndividualTriggerLevel(4, 0.4);
    //b->SetTriggerSource(15);

    b->SetTriggerDelayNs(pars.get<int>("delay"));             // zero ns trigger delay

    /* open file to save waveforms */
	std::string metadata = pars.get<std::string>("datadir")+pars.get<std::string>("filename")+".txt";
    f = fopen(metadata.c_str(), "w");
    if (f == NULL) {
        perror("ERROR: Cannot open file.");
        return 1;
    }
	fprintf(f, "drscmd version 1.0.0\n");
	fprintf(f, "N events required: %lu \n",pars.get<unsigned long int>("num_of_evt"));
	fprintf(f, "Trigger channel: %i \n",pars.get<int>("trigger_ch"));
	fprintf(f, "Trigger level (V): %f \n",trigger);
	fprintf(f, "Trigger Edge (neg or pos): %i \n",pars.get<bool>("edge_neg"));
	fprintf(f, "Trigger Delay: %i \n",pars.get<int>("delay"));
	fprintf(f, "Sampling Rate (GHz): %f \n",pars.get<double>("sampling"));
	fprintf(f, "RC Center (mV): %f \n",center);

	if(pars.exist("reset")){
		printf("Reset succeeded.");
		return 0;
	}

	unsigned long int n_event = 0;
	int events_per_file = 1000; // 1000
	printf("DAQ Start");

	struct timeval tv;

    for (n_event=0 ; n_event < pars.get<unsigned long int>("num_of_evt"); n_event++) {

		if(n_event % events_per_file == 0){
			if(n_event != 0) fclose(f_data);
			int n_loop = int(n_event / events_per_file);
			char num[5];
			sprintf(num, "%04u", n_loop);
			std::string n_loop_str (num);
			std::string f_data_name = pars.get<std::string>("datadir")+pars.get<std::string>("filename")+"."
					+n_loop_str+".dat";
			f_data = fopen(f_data_name.c_str(), "w");
		}

        /* start board (activate domino wave) */
        b->StartDomino();

        /* wait for trigger */
        //printf("Waiting for trigger...");

        //fflush(stdout);
        while (b->IsBusy());

		gettimeofday(&tv,NULL);
        /* read all waveforms */
        b->TransferWaves(0, 8);

        /* read time (X) array of first channel in ns */
        b->GetTime(0, 0, b->GetTriggerCell(0), time_array[0]);

        /* decode waveform (Y) array of first channel in mV */
        b->GetWave(0, 0, wave_array[0]);

        /* read time (X) array of second channel in ns
Note: On the evaluation board input #1 is connected to channel 0 and 1 of
the DRS chip, input #2 is connected to channel 2 and 3 and so on. So to
get the input #2 we have to read DRS channel #2, not #1. */
        b->GetTime(0, 2, b->GetTriggerCell(0), time_array[1]);

        /* decode waveform (Y) array of second channel in mV */
        b->GetWave(0, 2, wave_array[1]);

        b->GetTime(0, 4, b->GetTriggerCell(0), time_array[2]);
        b->GetWave(0, 4, wave_array[2]);

        b->GetTime(0, 6, b->GetTriggerCell(0), time_array[3]);
        b->GetWave(0, 6, wave_array[3]);

        /* Save waveform: X=time_array[i], Yn=wave_array[n][i] */
        fprintf(f_data, "%ld %li %i\n", n_event, tv.tv_sec, tv.tv_usec);
        for (i=0 ; i<1024 ; i++)
            fprintf(f_data, "%7.3f %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f %7.3f \n",
            time_array[0][i], wave_array[0][i], time_array[1][i], wave_array[1][i],
            time_array[2][i], wave_array[2][i], time_array[3][i], wave_array[3][i]
            );

        /* print some progress indication */
        if(n_event % 1000 == 0) printf("\rEvent #%ld read successfully\n", n_event);
    }

	printf("\r Total #%ld events read successfully\n", n_event);
	
    fclose(f);
    fclose(f_data);

    /* delete DRS object -> close USB connection */
    delete drs;
}
