
#include "sorting.h"
#include "stdlib.h"

void sort(struct darray* arr, int select){

  switch(select){
    case 2: insertion_sort(arr); break;
    case 3: quick_sort(arr); break;
    case 4: bucketSort(arr); break;
    case 5: mergeSort(arr,0,arr->size - 1); break;
    case 6: shellSort(arr); break; // Add your own choices here
    defualt:
      /* fprintf(stderr,
              "The value %d is not supported in sorting.c\n",
              select); */
      // Keep this exit code as the tests treat 23 specially
      exit(23);
  }
}


// You may find this helpful
void swap(char* *a, char* *b)
{
        char* temp = *a;
        *a = *b;
        *b = temp;
}


void insertion_sort(struct darray* arr)
{
  for(int i = 1; i < arr->size; i++)
  {
     Value_Type keyValue = arr->cells[i];

     int j = i - 1;
     while(j >= 0 && strcmp(arr->cells[j], keyValue) > 0)
     {
        arr->cells[j + 1] = arr->cells[j];
        arr->cells[j] = keyValue;
        j--;
     }
     
  }
}



int partition(struct darray* arr, int leftRange, int rightRange)
{
   Value_Type compareElement = arr->cells[rightRange];
   int i = leftRange - 1;
   for(int j = leftRange; j <= rightRange - 1; j++)
   {
     if(strcmp(arr->cells[j], compareElement) <= 0)
     {
       i++;
       swap(arr->cells[i], arr->cells[j]);
     }
   }

  swap(arr->cells[i+1], arr->cells[rightRange]);

  return (i+1);
}



void quick_sort_real(struct darray* arr, int leftRange, int rightRange)
{
   int q;
   if(leftRange < rightRange)
   {
      q = partition(arr, leftRange, rightRange);
      quick_sort_real(arr, leftRange, q - 1);
      quick_sort_real(arr, q + 1, rightRange);
    }
}


// Hint: you probably want to define a helper function for the recursive call
void quick_sort(struct darray* arr)
{
  printf(arr->cells[0]);
  quick_sort_real(arr, 0, arr->size - 1);
}


void bucketSort(struct darray* arr) 
{ 
    // 1) Create n empty buckets 
    Value_Type* subArray[26][arr->size];
    int lastPositionOfSubArray[26];
    

    for(int i = 0; i < 26;i++)
      lastPositionOfSubArray[i] = 0; 
    
    for (int i=0; i<arr->size; i++) 
    { 
       char* str = arr->cells[i];
       char c = str[0];
       switch(c)
       {
         case 'a': subArray[0][lastPositionOfSubArray[0]] = arr->cells[i]; lastPositionOfSubArray[0]++; break;
	 case 'b': subArray[1][lastPositionOfSubArray[1]] = arr->cells[i]; lastPositionOfSubArray[1]++; break;
	 case 'c': subArray[2][lastPositionOfSubArray[2]] = arr->cells[i]; lastPositionOfSubArray[2]++; break;
	 case 'd': subArray[3][lastPositionOfSubArray[3]] = arr->cells[i]; lastPositionOfSubArray[3]++; break;
	 case 'e': subArray[4][lastPositionOfSubArray[4]] = arr->cells[i]; lastPositionOfSubArray[4]++; break;
	 case 'f': subArray[5][lastPositionOfSubArray[5]] = arr->cells[i]; lastPositionOfSubArray[5]++; break;
	 case 'g': subArray[6][lastPositionOfSubArray[6]] = arr->cells[i]; lastPositionOfSubArray[6]++; break;
	 case 'h': subArray[7][lastPositionOfSubArray[7]] = arr->cells[i]; lastPositionOfSubArray[7]++; break;
	 case 'i': subArray[8][lastPositionOfSubArray[8]] = arr->cells[i]; lastPositionOfSubArray[8]++; break;
	 case 'j': subArray[9][lastPositionOfSubArray[9]] = arr->cells[i]; lastPositionOfSubArray[9]++; break;
	 case 'k': subArray[10][lastPositionOfSubArray[10]] = arr->cells[i]; lastPositionOfSubArray[10]++; break;
	 case 'l': subArray[11][lastPositionOfSubArray[11]] = arr->cells[i]; lastPositionOfSubArray[11]++; break;
	 case 'm': subArray[12][lastPositionOfSubArray[12]] = arr->cells[i]; lastPositionOfSubArray[12]++; break;
	 case 'n': subArray[13][lastPositionOfSubArray[13]] = arr->cells[i]; lastPositionOfSubArray[13]++; break;
	 case 'o': subArray[14][lastPositionOfSubArray[14]] = arr->cells[i]; lastPositionOfSubArray[14]++; break;
	 case 'p': subArray[15][lastPositionOfSubArray[15]] = arr->cells[i]; lastPositionOfSubArray[15]++; break;
	 case 'q': subArray[16][lastPositionOfSubArray[16]] = arr->cells[i]; lastPositionOfSubArray[16]++; break;
	 case 'r': subArray[17][lastPositionOfSubArray[17]] = arr->cells[i]; lastPositionOfSubArray[17]++; break;
	 case 's': subArray[18][lastPositionOfSubArray[18]] = arr->cells[i]; lastPositionOfSubArray[18]++; break;
         case 't': subArray[19][lastPositionOfSubArray[19]] = arr->cells[i]; lastPositionOfSubArray[19]++; break;
  	 case 'u': subArray[20][lastPositionOfSubArray[20]] = arr->cells[i]; lastPositionOfSubArray[20]++; break;
	 case 'v': subArray[21][lastPositionOfSubArray[21]] = arr->cells[i]; lastPositionOfSubArray[21]++; break;
	 case 'w': subArray[22][lastPositionOfSubArray[22]] = arr->cells[i]; lastPositionOfSubArray[22]++; break;
	 case 'x': subArray[23][lastPositionOfSubArray[23]] = arr->cells[i]; lastPositionOfSubArray[23]++; break;
	 case 'y': subArray[24][lastPositionOfSubArray[24]] = arr->cells[i]; lastPositionOfSubArray[24]++; break;
	 case 'z': subArray[25][lastPositionOfSubArray[25]] = arr->cells[i]; lastPositionOfSubArray[25]++; break;
     } 
  }




  
    
    for (int i=0; i<26; i++) 
       insertion_sort2(subArray[i], lastPositionOfSubArray[i]); 



     int index = 0; 
    for (int i = 0; i < 26; i++) 
        if(lastPositionOfSubArray[i] != 0)
         for (int j = 0; j < lastPositionOfSubArray[i]; j++) 
          arr->cells[index++] = subArray[i][j]; 

} 

void insertion_sort2(Value_Type* arr, int sizeOfArray)
{
  if(sizeOfArray > 1)
  {
    for(int i = 1; i < sizeOfArray; i++)
    {
      Value_Type keyValue = arr[i];

      int j = i - 1;
      while(j >= 0 && strcmp(arr[j], keyValue) > 0)
      {
         arr[j + 1] = arr[j];
         arr[j] = keyValue;
         j--;
      }
    }
  }
}




void merge(struct darray* arr, int leftIndex, int middleIndex, int rightIndex) 
{ 
    int numberOfElementsInLeftArray = middleIndex - leftIndex + 1; 
    int numberOfElementsInRightArray =  rightIndex - middleIndex; 
  

    Value_Type* L[numberOfElementsInLeftArray]; 
    Value_Type* R[numberOfElementsInRightArray]; 

    for (int i = 0; i < numberOfElementsInLeftArray; i++) 
        L[i] = arr->cells[leftIndex + i]; 
    for (int j = 0; j < numberOfElementsInRightArray; j++) 
        R[j] = arr->cells[middleIndex + 1 + j]; 	

  
    int indexOfFirstSubArray = 0;
    int indexOfSecondSubArray = 0; 
    int indexOfMergedSubArray = leftIndex; 


    while (indexOfFirstSubArray < numberOfElementsInLeftArray &&  indexOfSecondSubArray < numberOfElementsInRightArray) 
    { 
        if (strcmp(L[indexOfFirstSubArray], R[indexOfSecondSubArray]) <= 0) 
        { 
            arr->cells[indexOfMergedSubArray] = L[indexOfFirstSubArray]; 
            indexOfFirstSubArray++; 
        } 
        else
        { 
            arr->cells[indexOfMergedSubArray] = R[indexOfSecondSubArray]; 
            indexOfSecondSubArray++; 
        } 
        indexOfMergedSubArray++; 
    }
  

   while (indexOfFirstSubArray < numberOfElementsInLeftArray) 
    { 
        arr->cells[indexOfMergedSubArray] = L[indexOfFirstSubArray]; 
        indexOfFirstSubArray++; 
        indexOfMergedSubArray++; 
    } 
  

    while (indexOfSecondSubArray < numberOfElementsInRightArray) 
    { 
        arr->cells[indexOfMergedSubArray] = R[indexOfSecondSubArray]; 
        indexOfSecondSubArray++; 
        indexOfMergedSubArray++; 
    }
} 
  

void mergeSort(struct darray* arr, int leftIndex, int rightIndex) 
{ 
    if (leftIndex < rightIndex) 
    { 
        int middleIndex = (int)((leftIndex + rightIndex)/2); 
  
        mergeSort(arr, leftIndex, middleIndex); 
        mergeSort(arr, middleIndex + 1, rightIndex); 
  
        merge(arr, leftIndex, middleIndex, rightIndex); 
    } 
} 

int shellSort(struct darray* arr) 
{ 
    for (int gap = arr->size/2; gap > 0; gap /= 2) 
    { 
 
        for (int i = gap; i < arr->size; i++) 
        { 

            Value_Type currentElement = arr->cells[i]; 
  
            int j;             
            for (j = i; j >= gap && strcmp(arr->cells[j - gap], currentElement) > 0; j -= gap) 
                arr->cells[j] = arr->cells[j - gap]; 

            arr->cells[j] = currentElement; 
        } 
    } 
    return 0; 
} 




