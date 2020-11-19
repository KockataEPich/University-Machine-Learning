if(Log2n(countBinary) == powerCount-1){ 
	printf("\r%d%% ", powerCount*5);
	fflush(stdout);
        /* ADD CODE IN HERE TO KEEP TRACK OF FRACTION OF ENUMERATION DONE */
        if(countBinary > powerTwo){
	  printf("#");
	  fflush(stdout);
	  powerTwo <<= 1; 
        }
	powerCount++;
      } 
