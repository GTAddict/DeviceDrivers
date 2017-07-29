#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>

#define TRUE 1
#define FALSE 0
int QUIT = FALSE;

pthread_mutex_t INUSE;

unsigned char byte;

FILE* PARLELPORT;

void *all_blink(void* arg)
{

	QUIT = FALSE;

	pthread_mutex_lock(&INUSE);

	while (1)
	
	{
	
		byte = 0xFF;
		
		fwrite(&byte, 1, 1, PARLELPORT);
		
		sleep(0.5);
		
		byte = 0;
		
		fwrite(&byte, 1, 1, PARLELPORT);
		
		sleep(0.5);
		
		if (QUIT == TRUE)
		
		{
		
			pthread_mutex_unlock(&INUSE);
		
			pthread_exit(NULL);
			
		}
			
	}
	
}

void *all_on(void* arg)
{

	QUIT = FALSE;

	pthread_mutex_lock(&INUSE);
	
	byte = 0xFF;

	fwrite(&byte, 1, 1, PARLELPORT);
	
	while (1)
	
	{
	
		if (QUIT == TRUE)
		
			break;
			
	}
		
	
	byte = 0;
		
	fwrite(&byte, 1, 1, PARLELPORT);
		
	pthread_mutex_unlock(&INUSE);
		
	pthread_exit(NULL);
			
}

void *one_one(void* arg)
{

	QUIT = FALSE;

	pthread_mutex_lock(&INUSE);
	
	byte = 1;
	
	while (1)
	
	{
		fwrite(&byte, 1, 1, PARLELPORT);
		
		byte <<= 1;
	
		if (QUIT == TRUE)
		
			break;
			
		if (byte == 0)
		
			byte = 1;
			
		sleep(0.5);
			
	}
		
	
	byte = 0;
		
	fwrite(&byte, 1, 1, PARLELPORT);
		
	pthread_mutex_unlock(&INUSE);
		
	pthread_exit(NULL);
			
}

void *random_t(void* arg)
{

	QUIT = FALSE;

	pthread_mutex_lock(&INUSE);
	
	time_t seconds;
	
	seconds = time(NULL);
	
	srand((unsigned)seconds);
	
	while(1)
	
	{
	
		byte = (char)(rand() % 256);

		fwrite(&byte, 1, 1, PARLELPORT);
		
		sleep(0.5);
		
		if (QUIT == TRUE)
			
			break;
			
	}
				
	byte = 0;
		
	fwrite(&byte, 1, 1, PARLELPORT);
		
	pthread_mutex_unlock(&INUSE);
		
	pthread_exit(NULL);
			
}

int main()
{

	char dummy;
	
	int opt;

	PARLELPORT = fopen("/dev/parlelport", "w");
	
	setvbuf(PARLELPORT, &dummy, _IONBF, 1);

	pthread_t start, all_lights_on, all_lights_blinking, one_by_one, random;
	
	pthread_mutex_init(&INUSE, NULL);

	pthread_create(&start, NULL, all_blink, NULL);
	
	while (1)
	
	{
	
		printf("Welcome to our LED show! Please select an option:\n");
		
		printf("1. All lights on\n2. All lights blinking\n3. One by one\n4. Random\n5. Exit\n");
		
		scanf("%d", &opt);
		
		if ((opt > 5) || (opt < 1))
		
		{
		
			printf("Sorry! That is outside the range. Please try again:\n");
			
			sleep(0.5);
			
			break;
			
		}
		
		QUIT = TRUE;
		
		switch(opt)
		
		{
		
			case 1:	pthread_create(&start, NULL, all_blink, NULL);
				
					break;
					
			case 2:	pthread_create(&all_lights_on, NULL, all_on, NULL);
			
					break;
					
			case 3: pthread_create(&one_by_one, NULL, one_one, NULL);
			
					break;
					
			case 4: pthread_create(&random, NULL, random_t, NULL);
			
					break;
					
			case 5: fclose(PARLELPORT);
			
					exit(0);
					
		}
		
	}
	
	return 0;
	
}
