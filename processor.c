/*
Joseph Malafronte
Program to process SQL queries from 3 given tables
*/


#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>



typedef struct row{
	int col[30];
	int numColsRow;
	int markedAsMatched;
	int combined;
} row;

row *newRow (){
	row *ret = malloc(sizeof *ret);
	ret->col[0] = 0;
	ret->numColsRow = 0;
	ret->markedAsMatched = 0;
	ret->combined = 0;
	return ret;
}

typedef struct table{
	char* tableName;
	int numCols;
	char* colNames[30];
	int numRows;
	row *rows[5000];
} table;


table *newTable(){
	table *ret = malloc(sizeof *ret);
	ret->numRows = 0;
	ret->numCols = 0;
	return ret;
}


typedef struct query{
	int getStar;   //Bool value that is 1 if there is a * and 0 if not
	char *getCols[30];
	int numGetCols;
	int hasCombine;
	char *combineTableName; //String that houses the name of the table to combine with
	char *on;   //String that houses the on clause
	char *from;   //string that indicates what table is being selected from
	int hasWhere;  //Bool value that is 1 if there is a where statement and 0 if not
	char *whereStatement; //String that houses the where statement
	int passesWhere;
	char *origStatement;
} query;


query *newQuery(){
	query *ret = malloc(sizeof *ret);

	ret->getStar = 0;
	ret->hasWhere = 0;
	ret->hasCombine = 0;
	ret->numGetCols = 0;
	ret->combineTableName = malloc(100);
	ret->on = malloc(100);
	ret->from = malloc(100);
	ret->whereStatement = malloc(1000);
	ret->passesWhere = 0;
	ret->origStatement = malloc(1500);
	return ret;
}


char *convertToLower(char *ins){
	char *word = malloc(200);
	strcpy(word,ins);
	for(int i = 0; word[i]; i++){
  		word[i] = tolower(word[i]);
	}
	return word;
}


void createTableFromFile(table* tab, FILE *fp){


    //Have to do this manually

    /*int counter = 0;
    char buf[1000];
    tab->rows[tab->numRows] = newRow();
	while( fscanf(fp, "%s", buf) != EOF )
	    {
	    	char *word = &buf[0];
	    	//Ignore first line
	    	if(c==0){
	    		counter++;
	    		if(counter == tab->numCols){
	    			c = 1;
	    			counter = 0;
	    		}
	    	}
	    	else{
	    		if(counter<tab->numCols){
	    			if(strcmp(word,"NULL") == 0 ) tab->rows[tab->numRows]->col[counter] = INT_MIN;
			        else tab->rows[tab->numRows]->col[counter] = atoi(word);
			        tab->rows[tab->numRows]->numColsRow++;
			        counter++;
			        if(counter == tab->numCols){
	        			tab->numRows++;
	        			tab->rows[tab->numRows] = newRow();
	        			counter = 0;
			        }
	    		}
	    	}
	    	
	    }*/


	char * line = NULL;
    size_t len = 0;
    ssize_t read;

	    
    //Read all the lines in the file
    while ((read = getline(&line, &len, fp)) != -1) {

		row *rowIns = newRow();
        char *ar;

        


        //Use strtok function to split string along commas
        int i = 0;
        ar = strtok(line,",");
        if(strcmp(ar,"NULL") == 0 ) rowIns->col[0] = INT_MIN;
        else rowIns->col[0] = atoi(ar);
        rowIns->numColsRow++;
        while (ar != NULL) {
          i++;
          ar = strtok(NULL, ",");
          if(ar!=NULL){
          	if(strcmp(ar,"NULL") == 0 ) rowIns->col[i] = INT_MIN;
        	else rowIns->col[i] = atoi(ar);
          	rowIns->numColsRow++;
          }

        }

        tab->rows[tab->numRows] = rowIns;
        tab->numRows++;
    	
    }
}



void printTable(table* tab, FILE* fp){


	//printf("%s:\n", tab->tableName);
	int i =0;

	for(i=0;i<tab->numCols;i++){
		fprintf(fp, "%-9s", tab->colNames[i]);
	}
	fprintf(fp,"\n");

	for(i=0;i<tab->numRows;i++){
		row *curRow = tab->rows[i];
		int j = 0;
		for(j=0;j<tab->numCols;j++){
			if(curRow->col[j] == INT_MIN) fprintf(fp,"%-9s", " ");
			else fprintf(fp,"%-9d", curRow->col[j]);
		}
		fprintf(fp,"\n");
	}

	fprintf(fp,"\n");

}


table* createT1(table* a, table*b){

	table* t1 = newTable();
	t1->tableName = "T1";

	t1->colNames[0] = a->colNames[0];
	t1->colNames[1] = a->colNames[1];
	t1->colNames[2] = a->colNames[2];
	t1->colNames[3] = a->colNames[3];
	t1->colNames[4] = b->colNames[2];
	t1->colNames[5] = b->colNames[3];
	t1->colNames[6] = b->colNames[4];

	t1->numCols = 7;

	//Go through all rows in a, if any equal a row in b combine the rows.
	int i = 0;
	for(i = 0; i<a->numRows;i++){
		row *insRow = newRow();
		row *aRow = a->rows[i];
		int j = 0;
		for(j=0; j<b->numRows;j++){
			row *bRow = b->rows[j];
			//If they have the same primary keys
			if(aRow->col[0] == bRow->col[0] && aRow->col[1] == bRow->col[1]){
				aRow->markedAsMatched = 1;
				bRow->markedAsMatched = 1;
				
				insRow->col[0] = aRow->col[0];
				insRow->col[1] = aRow->col[1];
				insRow->col[2] = aRow->col[2];
				insRow->col[3] = aRow->col[3];
				insRow->col[4] = bRow->col[2];
				insRow->col[5] = bRow->col[3];
				insRow->col[6] = bRow->col[4];
				insRow->numColsRow = 7;

				t1->rows[t1->numRows] = insRow;
				t1->numRows++;
			}
		}
	}

	//Add all the rows in a if they didnt get matched
	for(i = 0; i<a->numRows;i++){
		row *aRow = a->rows[i];
		if(aRow->markedAsMatched == 0){
			row *insRow = newRow();
			insRow->col[0] = aRow->col[0];
			insRow->col[1] = aRow->col[1];
			insRow->col[2] = aRow->col[2];
			insRow->col[3] = aRow->col[3];
			insRow->col[4] = INT_MIN;
			insRow->col[5] = INT_MIN;
			insRow->col[6] = INT_MIN;
			insRow->numColsRow = 7;

			t1->rows[t1->numRows] = insRow;
			t1->numRows++;
		}
	}


	//Add all the rows in b if they didnt get matched
	for(i = 0; i<b->numRows;i++){
		row *bRow = b->rows[i];
		if(bRow->markedAsMatched == 0){
			row *insRow = newRow();
			insRow->col[0] = bRow->col[0];
			insRow->col[1] = bRow->col[1];
			insRow->col[2] = INT_MIN;
			insRow->col[3] = INT_MIN;
			insRow->col[4] = bRow->col[2];
			insRow->col[5] = bRow->col[3];
			insRow->col[6] = bRow->col[4];
			insRow->numColsRow = 7;

			t1->rows[t1->numRows] = insRow;
			t1->numRows++;
		}
	}


	return t1;
}


table* combineTables(table*a, table*b, char* on){



	table *returnTable = newTable();

	//Assign Col Names
	for(int i = 0; i<a->numCols;i++){
		returnTable->colNames[returnTable->numCols] = a->colNames[i];
		returnTable->numCols++;
	}
	for(int i = 0; i<b->numCols;i++){
		returnTable->colNames[returnTable->numCols] = b->colNames[i];
		returnTable->numCols++;
	}




	int aSelector = 0;
	int bSelector = 0;


	//Use strtok function to split string along equal signs
    int i = 0;
    char *item;
    item = strtok(on,"=");
    char *left = malloc(200);
    char *right = malloc(200);
    strcpy(left,item);

    item = strtok(NULL, ",");
    strcpy(right,item);


    for(int i = 0; i<a->numCols;i++){
    	if(strcmp(left, a->colNames[i]) == 0){
    		aSelector = i;
    	}
    	if(strcmp(right, a->colNames[i]) == 0){
    		aSelector = i;
    	}
    }
    for(int i = 0; i<b->numCols;i++){
    	if(strcmp(left, b->colNames[i]) == 0){
    		bSelector = i;
    	}
    	if(strcmp(right, b->colNames[i]) == 0){
    		bSelector = i;
    	}
    }
    

	returnTable->rows[returnTable->numRows] = newRow();


	//Go through all rows in a, if any equal a row in b combine the rows.
	for(i = 0; i<a->numRows;i++){
		row *aRow = a->rows[i];
		int j = 0;
		for(j=0; j<b->numRows;j++){
			row *bRow = b->rows[j];
			//If they have the same selectors
			if(aRow->col[aSelector] == bRow->col[bSelector]){
				int aCounter = 0;
				int bCounter = 0;
				for(int x = 0; x<returnTable->numCols;x++){
					if(aCounter<a->numCols){
						returnTable->rows[returnTable->numRows]->col[x] = aRow->col[aCounter];
						returnTable->rows[returnTable->numRows]->numColsRow++;
						aCounter++;
					}
					else if(bCounter<b->numCols){
						returnTable->rows[returnTable->numRows]->col[x] = bRow->col[bCounter];
						returnTable->rows[returnTable->numRows]->numColsRow++;
						bCounter++;
					}
				}

				if(returnTable->rows[returnTable->numRows]->combined==0){
					returnTable->rows[returnTable->numRows]->combined = 1;
					returnTable->numRows++;
					returnTable->rows[returnTable->numRows] = newRow();

				}
			}
		}
	}

	return returnTable;

}


int checkIfInt(char* word){
	for(int i =0; i<strlen(word);i++){
		if(!isdigit(word[i])) return 0;
	}
	return 1;
}

char *emptyString(char *word){
	for(int i = 0; i<strlen(word);i++){
		word[i] = '\0';
	}
	return word;
}

int checkOperation(table *tab, row *curRow, char*clause){


	char* leftName = malloc(100);
	char* rightName = malloc(100);
	char* operation;
	//int count;
	char *runner = malloc(100);
	char *rightRunner = malloc(100);
	//char *emptyString = malloc(100);
	int runCount = 0;
	int rightRunnerCount = 0;

	for(int i = 0;i<strlen(clause);i++){
		if(clause[i] == '<'){
			strcpy(leftName,runner);
			runner = emptyString(runner);
			runCount = 0;
			rightRunnerCount =1;
			if(clause[i+1] == '='){
				operation = "<=";
				i++;
			}
			else if(clause[i+1] == '>'){
				operation = "<>";
				i++;
			}
			else{
				operation = "<";
			}
		}
		else if (clause[i] == '>'){
			strcpy(leftName,runner);
			runner = emptyString(runner);
			runCount = 0;
			rightRunnerCount =1;
			if(clause[i+1] == '='){
				operation = ">=";
				i++;
			}
			else{
				operation = ">";
			}
		}
		else if (clause[i] == '='){
			strcpy(leftName,runner);
			runCount = 0;
			rightRunnerCount =1;
			operation = "=";
		}
		else if(rightRunnerCount == 0) {
			runner[runCount] = clause[i];
			runCount++;
		}
		else {
			rightRunner[runCount] = clause[i];
			runCount++;
		}
	}

	rightName = rightRunner;
	int rightInt = INT_MIN;

	if(checkIfInt(rightName)){
		rightInt = atoi(rightName);
	} 



	//if right term is number
	if(rightInt != INT_MIN){
		int corNum = 0;

		

		//Check table for col name
		for(int i = 0; i<tab->numCols;i++){
			if(strcmp(leftName,tab->colNames[i]) == 0){
				corNum = i;
			}
		}

		if(curRow->col[corNum] == INT_MIN){
			if(curRow->col[corNum] != rightInt && strcmp(operation,"<>")== 0) return 1;
			else return 0;
		}

		if(strcmp(operation,"<=")== 0){
			if(curRow->col[corNum] <= rightInt) return 1;
			else return 0;
		} 
		else if(strcmp(operation,"<>")== 0){
			if(curRow->col[corNum] != rightInt) return 1;
			else return 0;
		}
		else if(strcmp(operation,"<")== 0){
			if(curRow->col[corNum] < rightInt) return 1;
			else return 0;
		} 
		else if(strcmp(operation,">=")== 0){
			if(curRow->col[corNum] >= rightInt) return 1;
			else return 0;
		}   
		else if(strcmp(operation,">")== 0){
			if(curRow->col[corNum] > rightInt) return 1;
			else return 0;
		} 
		else if(strcmp(operation,"=")== 0){
			if(curRow->col[corNum] == rightInt) return 1;
			else return 0;
		}   
	} 

	//if right term is colName
	else if(rightInt == INT_MIN){
		int corNum = 0;
		int corNumR = 0;


		//Check table for col name
		for(int i = 0; i<tab->numCols;i++){
			if(strcmp(leftName,tab->colNames[i]) == 0){
				corNum = i;
			}
			if(strcmp(rightName,tab->colNames[i]) == 0){
				corNumR = i;
			}
		}

		if(curRow->col[corNum] == INT_MIN || curRow->col[corNumR] == INT_MIN){
			if(curRow->col[corNum] != curRow->col[corNumR] && strcmp(operation,"<>")== 0) return 1;
			if(curRow->col[corNum] == curRow->col[corNumR] && strcmp(operation,"=")== 0) return 1;
			else return 0;
		}

		if(strcmp(operation,"<=")== 0){
			if(curRow->col[corNum] <= curRow->col[corNumR]) return 1;
			else return 0;
		} 
		else if(strcmp(operation,"<>")== 0){
			if(curRow->col[corNum] != curRow->col[corNumR]) return 1;
			else return 0;
		}
		else if(strcmp(operation,"<")== 0){
			if(curRow->col[corNum] < curRow->col[corNumR]) return 1;
			else return 0;
		} 
		else if(strcmp(operation,">=")== 0){
			if(curRow->col[corNum] >= curRow->col[corNumR]) return 1;
			else return 0;
		}   
		else if(strcmp(operation,">")== 0){
			if(curRow->col[corNum] > curRow->col[corNumR]) return 1;
			else return 0;
		} 
		else if(strcmp(operation,"=")== 0){
			if(curRow->col[corNum] == curRow->col[corNumR]) return 1;
			else return 0;
		}   
	} 

	return 0;
}


void append(char *subject, char *insert, int pos) {
    char buf[300] = {}; // 100 so that it's big enough. fill with zeros
    // or you could use malloc() to allocate sufficient space
    // e.g. char *buf = (char*)malloc(strlen(subject) + strlen(insert) + 2);
    // to fill with zeros: memset(buf, 0, 100);

    strncpy(buf, subject, pos); // copy at most first pos characters
    int len = strlen(buf);
    strcpy(buf+len, insert); // copy all of insert[] at the end
    len += strlen(insert);  // increase the length by length of insert[]
    strcpy(buf+len, subject+pos); // copy the rest

    strcpy(subject, buf);   // copy it back to subject
    // Note that subject[] must be big enough, or else segfault.
    // deallocate buf[] here, if used malloc()
    // e.g. free(buf);
}


//Send a table a row and a where clause and this will tell you if the row passes or not
int runThroughWhere (table *tab, row *curRow, char *clauseA){


	char *clause = malloc(300);
	strcpy(clause, clauseA);

	//First remove opening and closing paranthesis
	char *newClause = &clause[1];
	newClause[strlen(newClause)-1] = '\0';

	char *saveClause = malloc(300);
	strcpy(saveClause,newClause);

	char *runClause = malloc(300);
	strcpy(runClause,newClause);

	int boolRecorder[2];
	int numBools = 0;


	int opCounter = 0;
	int paCounter = 0;
	int flag = 0;

	//Check if more than 2 terms
	for(int i=0;i<strlen(runClause);i++){
		if(flag == 1) break;
		if(runClause[i] == '('){
			paCounter++;
		}
		if(runClause[i] == ')'){
			paCounter--;
		}
		if(paCounter==0 && tolower(runClause[i]) == 'a' && tolower(runClause[i+1]) == 'n' && tolower(runClause[i+2]) == 'd'){
			opCounter++;
			if(opCounter==2){
				append(runClause, "(", 0);
				append(runClause, ")", i);
				append(runClause, "(", i+6);
				append(runClause, ")", strlen(runClause));
				newClause = runClause;
				flag = 1;
			}
		}
		if(paCounter==0 && tolower(runClause[i]) == 'o' && tolower(runClause[i+1]) == 'r'){
			opCounter++;
			if(opCounter==2){
				append(runClause, "(", 0);
				append(runClause, ")", i);
				append(runClause, "(", i+5);
				append(runClause, ")", strlen(runClause));
				newClause = runClause;
				flag = 1;
			}
		}
	}



	int isAnd = 0;
	int isOr = 0;

	int hasParan = 0;
	int runRecorder = 1;
	char *runner = &newClause[0];
	char *rightRunner = malloc(500);
	strcpy(rightRunner,runner);
	char *recorder = malloc(500);
	char *rightRecorder = malloc(500);
	int recCounter = 0;
	int pCounter = 0;


	//Get Left Paran and Right Paran
	for(int i =0; i<strlen(runner);i++){

		if(runRecorder == 1){
			recorder[recCounter] = runner[i];
			if(runner[i] == '(') pCounter++;
			else if(runner[i] == ')'){
				hasParan = 1;
				pCounter--;
				if(pCounter == 0) {
					boolRecorder[numBools] = runThroughWhere(tab, curRow, recorder);
					char empty[500];
					recorder = &empty[500];
					numBools++;
					recCounter = -1;
					runRecorder = 0;
				}
			}

			recCounter++;
		}
		else if(runRecorder == 2){
			rightRecorder[recCounter] = rightRunner[i];
			if(rightRunner[i] == '(') pCounter++;
			else if(rightRunner[i] == ')'){
				hasParan = 1;
				pCounter--;
				if(pCounter == 0) {
					boolRecorder[numBools] = runThroughWhere(tab, curRow, rightRecorder);
					numBools++;
					recCounter = 0;
					runRecorder = 0;
				}
			}

			recCounter++;
		}
		else if(runner[i] == '('){
			pCounter++;
			rightRecorder[0] = rightRunner[i];
			recCounter = 1;
			runRecorder = 2;
		}

		if(runRecorder == 0 && tolower(runner[i]) == 'a'){
			isAnd = 1;
		} 
		if(runRecorder == 0 && tolower(runner[i]) == 'o') isOr = 1;
		
	}

	char *leftItem = malloc(200);
	char *rightItem = malloc(200);
	char *operator = malloc(200);
	int hasMultiple = 0;

	if(hasParan == 1){
		if(isAnd == 1){
			if(boolRecorder[0] && boolRecorder[1]) return 1;
			else return 0;
		}
		if(isOr == 1){
			if(boolRecorder[0] || boolRecorder[1]) return 1;
			else return 0;
		}
	}
	//No parans base case
	else{

		//Use strtok function to split string along spaces
        int i = 0;
        char *item;
        item = strtok(saveClause," ");
        strcpy(leftItem,item);

        while (item != NULL) {
          i++;
          item = strtok(NULL," ");
          if(item!=NULL){
	      	if(hasMultiple == 0) strcpy(operator,item);
	      	else strcpy(rightItem,item);
	  		hasMultiple=1;
          }

        }	


        if(hasMultiple == 0){
        	int test = checkOperation(tab, curRow, leftItem);
        	return test;
        }
        else{
        	int test1 = checkOperation(tab, curRow, leftItem);
        	int test2 = checkOperation(tab, curRow, rightItem);
        	if(strcmp(convertToLower(operator),"and") == 0){
        		if(test1 && test2) return 1;
        		else return 0;
        	}
        	else if(strcmp(convertToLower(operator),"or") == 0){
        		if(test1 || test2) return 1;
        		else return 0;
        	}
        }

	}


	return 0;
}


table* processQuery(query* query, table *allTables[20], int numTables){
	


	table *selectedTable;
	table *tableToGet;
	table *tableToReturn;
	char* lowerTest = convertToLower(query->from);


	//Handle From Clause
	if(query->hasCombine == 0){
		for(int i = 0;i<numTables;i++){
			if(strcmp(lowerTest,convertToLower(allTables[i]->tableName))==0){
				selectedTable = allTables[i];
			}
		}
	}
	else{
		table *firstTable;
		table *secondTable;
		char *lowerTest2 = convertToLower(query->combineTableName);
		for(int i = 0;i<numTables;i++){
			if(strcmp(lowerTest,convertToLower(allTables[i]->tableName))==0){
				firstTable = allTables[i];
			}
			if(strcmp(lowerTest2,convertToLower(allTables[i]->tableName))==0){
				secondTable = allTables[i];
			}
		}

		selectedTable = combineTables (firstTable,secondTable, query->on);
	}



	//Handle Where Clause
	if(query->hasWhere == 0) tableToGet = selectedTable;
	else{
		tableToGet = newTable();
		//Copy over Column info
		for(int i=0;i<selectedTable->numCols;i++){
			tableToGet->colNames[i] = selectedTable->colNames[i];
			tableToGet->numCols++;
		}

		for(int i =0;i<selectedTable->numRows;i++){
			int check = runThroughWhere(selectedTable, selectedTable->rows[i],query->whereStatement);
			//if row passes
			if(check){
				tableToGet->rows[tableToGet->numRows] = selectedTable->rows[i];
				tableToGet->numRows++;
			}
		}

	}



	//Handle Get Clause
	if(query->getStar == 1){
		tableToReturn = tableToGet;
	} 
	else{

		tableToReturn = newTable();
		tableToReturn->numCols = query->numGetCols;
		for(int i = 0;i<query->numGetCols;i++){
			tableToReturn->colNames[i] = query->getCols[i];
		}

		//Go through all the rows in the last table and create new rows for the new table with only the desired data
		for(int i = 0;i<tableToGet->numRows;i++){
			row *insRow = newRow();
			insRow->numColsRow = tableToReturn->numCols;
			int goodForInsert = 0;

			row *compRow = tableToGet->rows[i];

			//Go through all the new column colNames
			for(int j = 0; j<tableToReturn->numCols;j++){
				char* newColName = tableToReturn->colNames[j];

				//Compare to all the old column names
				for (int x = 0; x<tableToGet->numCols;x++){
					char* oldColName = tableToGet->colNames[x];
					if(strcmp(newColName,oldColName)==0){
						goodForInsert = 1;
						insRow->col[j] = compRow->col[x];
						
					}
				}
			}

			if(goodForInsert == 1){
				tableToReturn->rows[tableToReturn->numRows] = insRow;
				tableToReturn->numRows++;
			}
		}
	}





	return tableToReturn;
}


char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the null-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


int main(int argc, char *argv[])
{

	table *allTables[20];
	int numTables = 0;

	FILE * fp;

	//Create and Read Table T1.A
	table *t1a = newTable();
	t1a->tableName = "T1A";
	t1a->colNames[0] = "K1";
	t1a->colNames[1] = "TS";
	t1a->colNames[2] = "C1";
	t1a->colNames[3] = "C2";
	t1a->numCols = 4;

    fp = fopen("T1A.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    createTableFromFile(t1a, fp);

    allTables[0] = t1a;

    fclose(fp);

    
    //Create and Read Table T1.B
	table *t1b = newTable();
	t1b->tableName = "T1B";
	t1b->colNames[0] = "K1";
	t1b->colNames[1] = "TS";
	t1b->colNames[2] = "C3";
	t1b->colNames[3] = "C4";
	t1b->colNames[4] = "C5";
	t1b->numCols = 5;


    fp = fopen("T1B.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    createTableFromFile(t1b, fp);

    allTables[1] = t1b;

    fclose(fp);


    //Create Table T1 
    table *t1 = createT1(t1a, t1b);
    allTables[2] = t1;


    //Create and Read Table T2
	table *t2 = newTable();
	t2->tableName = "T2";
	t2->colNames[0] = "K2";
	t2->colNames[1] = "TS2";
	t2->colNames[2] = "A1";
	t2->colNames[3] = "A2";
	t2->colNames[4] = "A3";
	t2->numCols = 5;


    fp = fopen("T2.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    createTableFromFile(t2, fp);

    allTables[3] = t2;
    numTables = 4;

    fclose(fp);


    /*
    printTable(allTables[0]);
    printTable(allTables[1]);
    printTable(allTables[2]);
    printTable(allTables[3]);
	*/


    //Get all queries
    query *allQueries[100];
    int numQueries = 0;

    FILE *fpR= fopen("queries.txt","r");
 
    char buf[1000];
    int comingFromGet = 0;
    int comingFromFrom = 0;
    int comingFromCombine = 0;
    int comingFromOn = 0;
    int comingFromWhere = 0;
    char *whereStatement;
    int whereLeaveOff = 0;
    int notFirstWhere = 0;
    allQueries[numQueries] = newQuery();
    whereStatement = allQueries[numQueries]->whereStatement;
    int hasSemi = 0;
    while( fscanf(fpR, "%s", buf) != EOF )
    {
    	char *word = &buf[0];

    	strcat(allQueries[numQueries]->origStatement, word);
    	strcat(allQueries[numQueries]->origStatement, " ");


    	if(word[strlen(word)-1] == ';'){
    		hasSemi = 1;
    		word[strlen(word)-1] = '\0';
    	}


		if(strcmp(convertToLower(word),"where") == 0){
    		comingFromCombine = 0;
    		comingFromFrom = 0;
    		comingFromOn = 0;
    		comingFromWhere = 1;
    		allQueries[numQueries]->hasWhere = 1;
    	}
    	else if(comingFromWhere == 1){
    		if(notFirstWhere == 1){
    				whereStatement[whereLeaveOff] = ' ';
    				whereStatement[whereLeaveOff] = ' ';
    				whereLeaveOff++;
    			}
			else notFirstWhere = 1;
    		for(int i=0;i<strlen(word);i++){
    			whereStatement[whereLeaveOff] = word[i];
    			whereLeaveOff++;
    		}
    		//comingFromWhere = 1;
    	}
    	else if(strcmp(convertToLower(word),"on") == 0){
    		comingFromCombine = 0;
    		comingFromOn = 1;
    	}
    	else if(comingFromOn == 1){
    		strcpy(allQueries[numQueries]->on,word);
    		comingFromOn = 0;
    	}
    	else if(strcmp(convertToLower(word),"combine") == 0){
    		allQueries[numQueries]->hasCombine = 1;
    		comingFromFrom = 0;
    		comingFromCombine = 1;
    	}
    	else if(comingFromCombine == 1){
    		strcpy(allQueries[numQueries]->combineTableName,word);
    		comingFromCombine = 0;
    	}
    	else if(strcmp(convertToLower(word),"from") == 0){
    		comingFromGet = 0;
    		comingFromFrom = 1;
    	}
    	else if(comingFromFrom == 1){
    		strcpy(allQueries[numQueries]->from,word);
    		comingFromFrom = 0;
    	}
    	else if(strcmp(convertToLower(word),"get") == 0){
    		comingFromGet = 1;
    	}
    	else if(comingFromGet == 1){
    		if(strcmp(word, "*") == 0){
    			allQueries[numQueries]->getStar = 1;
    			comingFromGet = 0;
    		}
    		else{
    			if(word[strlen(word)-1] == ','){
		    		word[strlen(word)-1] = '\0';
		    	}
		    	query *thisQuery = allQueries[numQueries];
		        thisQuery->getCols[thisQuery->numGetCols] = malloc(200);
		        strcpy(thisQuery->getCols[thisQuery->numGetCols], word);
		        thisQuery->numGetCols++;
    		}
    	}


    	if(hasSemi == 1){
    		numQueries++;
    		allQueries[numQueries] = newQuery();
    		
    		//Verify where statement
    		if(whereStatement[0] == ' ' && whereStatement[1]=='('){
    			memmove(whereStatement, whereStatement+1, strlen(whereStatement));
    		}


    		whereStatement = allQueries[numQueries]->whereStatement;
    		whereLeaveOff = 0;
    		hasSemi = 0;
    		 	 comingFromGet = 0;
			     comingFromFrom = 0;
			     comingFromCombine = 0;
			     comingFromOn = 0;
			     comingFromWhere = 0;
    	}
    }

    FILE *fpo = fopen("output.txt","w");


    fprintf(fpo,"\n");
    for(int i = 0; i<numQueries;i++){
    	table *queryRun = processQuery(allQueries[i],allTables,numTables);
    	fprintf(fpo, "%s\n", allQueries[i]->origStatement);
    	printTable(queryRun,fpo);
    }


    return 0;
}