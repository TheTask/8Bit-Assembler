#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define N_INSTRUCTIONS 16

struct dict 
{
    struct dict* next; 
    char* key; 
    char* value;
};

static struct dict* instructions[ N_INSTRUCTIONS ]; 


void init_opcodes();
char* to_4_bit_binary( int dec,char* output );
unsigned hash( char* s );
struct dict* get( char* s );
struct dict* put( char* key,char* value );
char* stringdup( char* s );


int main( int argc,char** argv )
{
	char instr[ 8 ];
	char instr_opcode[ 4 ];
	char instr_value[ 5 ];
      	char mem_addr_bin[ 5 ];
      	int mem_addr = 0;
	char* payload;			

	init_opcodes(); 
		
	while( fgets( instr,8,stdin ) != NULL )
	{ 
		if( strlen( instr ) == 1 || strlen( instr ) == 0 ) break; //for manual input

		unsigned int counter = 0;
			 
		//remove \n from the end        
		instr[ strcspn( instr,"\r\n" ) ] = 0;

		//separate opcode
		while( !( instr[ counter ] == ' ' || instr[ counter ] == '\0' ) )
		{
			instr_opcode[ counter ] = instr[ counter ];
			counter++;			
		}
		instr_opcode[ counter++ ] = '\0';

		to_4_bit_binary( mem_addr++,mem_addr_bin ); //convert and increment memory address
			
		struct dict* op = get( instr_opcode ); //retrieve the dict object that corresponds to the opcode
			
		if( op == NULL ) //opcode not found, pass on 
		{
			printf( "%s\t%s\t\t%s\n",mem_addr_bin,instr,instr );
		}
		else
		{	
			payload = &instr[ counter ]; //payload starts after the opcode
			to_4_bit_binary( (int)strtol( payload,NULL,10 ),instr_value ); //convret payload
        
			printf( "%s\t%s%s\t%s\n",mem_addr_bin,op->value,instr_value,instr );
		}
	}
	
	return 0;
}

void init_opcodes()
{
	put( "NOP","0000" );
	put( "LDA","0001" );
	put( "ADD","0010" );
	put( "SUB","0011" );
	put( "STA","0100" );
	put( "LDI","0101" );
	put( "JMP","0110" );
	put( "JC" ,"0111" );
	put( "JZ" ,"1000" );

	put( "OUT","1110" );
	put( "HLT","1111" );
}

char* to_4_bit_binary( int dec,char* output ) 
{
    output[ 4 ] = '\0';
    output[ 3 ] =   ( dec	 & 1 ) + '0';
    output[ 2 ] = ( ( dec >> 1 ) & 1 ) + '0';
    output[ 1 ] = ( ( dec >> 2 ) & 1 ) + '0';
    output[ 0 ] = ( ( dec >> 3 ) & 1 ) + '0';
    return output;
}

unsigned hash( char* s )
{
    	unsigned hashval;
    	for( hashval = 0; *s != '\0'; s++ ) hashval = *s + 31 * hashval;
    	return hashval % N_INSTRUCTIONS;
}

struct dict* get( char* s )
{
    	struct dict* np;
    	for( np = instructions[ hash( s ) ]; np != NULL; np = np->next )
    	{
        	if( strcmp( s,np->key ) == 0 ) return np; /* found */
	}
    	return NULL; /* not found */
}

struct dict* put( char* key,char* value )
{
    struct dict* np;
    unsigned hashval;

    	if( ( np = get( key ) ) == NULL )  /* not found */
        {
		np = (struct dict*)malloc( sizeof( *np ) );
        	
		if ( np == NULL || ( np->key = stringdup( key ) ) == NULL ) return NULL;
        
		hashval = hash( key );
       		np->next = instructions[ hashval ];
        	instructions[ hashval ] = np;
    	} 	
	else free( (void *)np->value ); /*free previous value */
    	
	if ( ( np->value = stringdup( value ) ) == NULL ) return NULL;
    	return np;
}

char* stringdup( char* s ) /* make a duplicate of s */
{
    	char* p;
    	p = (char*)malloc( strlen( s ) + 1 ); /* +1 for ’\0’ */
    
	if( p != NULL ) strcpy( p,s );
    	return p;
}
