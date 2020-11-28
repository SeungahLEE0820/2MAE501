
			
	data = (SensorData **) malloc( MAX * sizeof(SensorData *) );

	pthread_mutex_lock( &mutex );
	DBp = fopen("data1.txt", "r");
	if (DBp == NULL) {               // Verification

		printf("Unable to open file.\n");
		exit(EXIT_FAILURE);
	}
	
	int i = 0;
	while (fgets(line, MAXCHAR, DBp) != NULL){     // get lines until the end of the DB
	
		if (strlen(line) > 2) {                // Only if the line contains information
	
			// Allocate memory to save data in a new structure
			*(data + i) = (SensorData *) malloc(sizeof(SensorData));

			printf("%s\n",line);

			(*(data + i)) -> factory = (int) line[0] - 48;

			unsigned int j = 4;
			int k = 0;
			int l = 0;
			
			
			while (j<= strlen(line)) {
				
				
				if( (line[j] != 0x020) && (j<40) ){

					value[k] = line[j];
					j++;
					k++;

				} else {
					
					if (l == 0) {

						(*(data + i)) -> time = atoi(value);

					} else if (l == 1){

						(*(data + i)) -> temp = (float) strtod(value,NULL); 

					} else if (l == 2){

						(*(data + i)) -> press = (float) strtod(value,NULL);

					} else if (l == 3){

						(*(data + i)) -> hum = (float) strtod(value,NULL);
												}


					l++;
					j += 3;
					k = 0;
					memset(value, 0, 10);
				}
			}
			i++;
			l = 0; 
		}
	}

	
	for (int j=0; j<5; j++){
		printf("line %d, factory %d, time = %ld, T=%.2f, P=%.2f, h=%.2f\n", 
			j, 
			(*(data + j)) -> factory,
			(*(data + j)) -> time,
			(*(data + j)) -> temp, 
			(*(data + j)) -> press, 
			(*(data + j)) -> hum    );
	}

	 
	fclose(DBp);
	pthread_mutex_unlock ( &mutex );

	/* We have to deallocate memory*/
	int lines = i;
	for (i=0; i<lines; i++){
		free(*(data + i));

	}
	free(data);
	data = NULL;	
	
