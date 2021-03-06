/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;
int i = 0;
extern CharCode charCodes[];
int maxNumberDigit = 10;

/***************************************************************/

void skipBlank() {
  // TODO
  // have blank -> reset value to zero.
  while ((charCodes[currentChar] == CHAR_SPACE) && (currentChar != EOF))
  {
    readChar();
  }
}

void skipComment() {
  // TODO
  int checkEndOfComment = 0;
  // checkEndOfComment = 1- > char now get: *, char =2: ) => finish skip comment
  // Go to end of comment, have model: *)
  while ((currentChar != EOF) && (checkEndOfComment < 2))
  {
    switch (charCodes[currentChar])
    {
    case CHAR_TIMES:
    {
      checkEndOfComment++;
      break;
    }
    case CHAR_RPAR:
    {
      // if we had '*', now when get ')' => ok we have all conditions for an comment
      if (checkEndOfComment == 1)
      {
        checkEndOfComment = 2;
      }
      else
      {
        checkEndOfComment = 0;
      }
      break;
    }
    default:
      checkEndOfComment = 0;
    }
    readChar();
  }
  if (checkEndOfComment != 2)
  {
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
  }
}

Token* readIdentKeyword(void) {
  // TODO
  Token *tokenReadIdentKeyword = makeToken(TK_CHAR, lineNo, colNo);
  char stringSaveValue[MAX_IDENT_LEN + 1];
  // char s3[MAX_IDENT_LEN + 1];
  char convertCurrentCharToChar = currentChar;
  i = 0;
  stringSaveValue[i] = convertCurrentCharToChar;
  currentChar = readChar();
  while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)
  {
    i++;
    convertCurrentCharToChar = currentChar;
    stringSaveValue[i] = convertCurrentCharToChar;
    currentChar = readChar();
  }
  // strcpy(tokenReadIdentKeyword->string, stringSaveValue);
  for (int index = 0; index <= i; index++)
  {
    tokenReadIdentKeyword->string[index] = stringSaveValue[index];
  }
  if (i > MAX_IDENT_LEN)
  {
    error(ERR_IDENTTOOLONG, tokenReadIdentKeyword->lineNo, tokenReadIdentKeyword->colNo);
  }
  TokenType valueHere = checkKeyword(tokenReadIdentKeyword->string);
  if (valueHere != TK_NONE)
  {
    tokenReadIdentKeyword->tokenType = valueHere;
  }
  else
  {
    tokenReadIdentKeyword->tokenType = TK_IDENT;
  }
  return tokenReadIdentKeyword;

}

// Token* readNumber(void) {
//   // TODO
//   Token *tokenNumber = makeToken(TK_NUMBER, lineNo, colNo);
//   int i = 0;
//   char convertToNumber = currentChar;
//   while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT))
//   {
//     tokenNumber->string[i] = convertToNumber;
//     i++;
//     readChar();
//     convertToNumber = currentChar;
//   }
//   if (i > maxNumberDigit)
//   {
//     error(ERR_INVALIDNUMBER, lineNo, colNo);
//   }
//   else
//   {
//     return tokenNumber;
//   }
// } 
Token* readNumber(void) {
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int i = 0;
  int testFloat = 0;
  while (charCodes[currentChar] == CHAR_DIGIT || charCodes[currentChar] == CHAR_PERIOD)
  {
    if (charCodes[currentChar] == CHAR_PERIOD)
    {
      token->tokenType = TK_FLOAT;
      testFloat++;
    }
    if (i > maxNumberDigit)
    {
      error(ERR_INVALIDNUMBER, token->lineNo, token->colNo);
    }
    if (testFloat > 1)
      error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    token->string[i] = currentChar;
    i++;
    readChar();
  }
  token->string[i] = '\0';
  token->value = atoi(token->string);
  return token;

}
Token* readConstChar(void) {
  // TODO
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  readChar();
  if(currentChar == EOF){
    token -> tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token -> lineNo, token -> colNo);
    return token;

  }
  token -> string[0] = currentChar;
  token -> string[1] = '\0';
  token->value = currentChar;

  readChar();
  if(currentChar == EOF){
    token -> tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token -> lineNo, token -> colNo);
    return token;
  }
  if (charCodes[currentChar] == CHAR_SINGLEQUOTE){
    readChar();
    return token;
  }else
  {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
}
Token *readConstString()
{
  Token *token;
  token = makeToken(TK_STRING, lineNo, colNo);
  int i = 0;
  while (charCodes[readChar()] != CHAR_DOUBLEQUOTE)
  {
    token->string[i++] = currentChar;
    if (i == MAX_IDENT_LEN || currentChar == '\n')
    {
      error(ERR_INVALIDSTRINGCONSTANT, token->lineNo, token->colNo);
    }
  }
  token->string[i] = '\0';
  readChar();
  return token;
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
    // ....
    // TODO
    // ....
  case CHAR_MINUS: 
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar(); 
    return token;  
  case CHAR_TIMES:  
    token = makeToken(SB_TIMES, lineNo, colNo); 
    readChar(); 
    return token;
  case CHAR_SLASH:// token l?? d???u ???/??? 
    token = makeToken(SB_SLASH, lineNo, colNo); 
    readChar(); 
    return token;  
  
  case CHAR_LT: // g???p d???u ???<??? 
    ln = lineNo; 
    cn = colNo; 
    readChar(); // ki???m tra n???u ti???p theo l?? d???u ???=??? 
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) { 
      readChar(); 
      return makeToken(SB_LE, ln, cn); // token l?? d???u ???<=??? 
    } else return makeToken(SB_LT, ln, cn); // token l?? d???u ???<???
  case CHAR_GT: // g???p d???u ???>??? 
    ln = lineNo; 
    cn = colNo; 
    readChar(); // ki???m tra k?? t??? ti???p theo l?? d???u ???=??? 
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) { 
      readChar(); 
      return makeToken(SB_GE, ln, cn); // token l?? d???u ???>=??? 
    } else return makeToken(SB_GT, ln, cn); 
  case CHAR_EQ: // token l?? d???u ???=??? 
    token = makeToken(SB_EQ, lineNo, colNo); 
    readChar(); 
    return token; 
  case CHAR_EXCLAIMATION:// g???p d???u ???!??? 
    ln = lineNo; 
    cn = colNo; 
    readChar(); // ki???m tra k?? t??? ti???p l?? d???u ???=??? 
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) { 
      readChar(); 
      return makeToken(SB_NEQ, ln, cn); // token l?? d???u ???!=??? 
    } else { // n???u k?? t??? ti???p kh??ng ph???i l?? d???u ???=???, b??o l???i 
        token = makeToken(TK_NONE, ln, cn); 
        error(ERR_INVALIDSYMBOL, ln, cn); return token; 
    }
  case CHAR_COMMA: // token l?? d???u ???,??? 
    token = makeToken(SB_COMMA, lineNo, colNo); 
    readChar(); 
    return token;
  case CHAR_PERIOD: // g???p d???u ???.??? 
    ln = lineNo; 
    cn = colNo; 
    readChar(); // ki???m tra k?? t??? ti???p theo l?? d???u ???)??? 
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_RPAR)) { 
      readChar(); 
      return makeToken(SB_RSEL, ln, cn); // tr??? lai token l?? ???.)??? 
    } else if (charCodes[currentChar] == CHAR_DIGIT)//test float
    {
      // token->tokenType = TK_FLOAT;
      token = makeToken(TK_FLOAT, lineNo, colNo); 
      int count = 2;
      token->string[0] = '0';
      token->string[1] = '.';
      while (charCodes[currentChar] == CHAR_DIGIT)
      {
        if (count > maxNumberDigit)
        {
          error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
        }
        token->string[count] = currentChar;
        count++;
        readChar();
      }
      token->string[count] = '\0';
      token->value = atoi(token->string);
      return token;
    }else return makeToken(SB_PERIOD, ln, cn); // tr??? l???i token l?? d???u ???.??? 
  case CHAR_SEMICOLON: // g???p d???u ???;??? 
    token = makeToken(SB_SEMICOLON, lineNo, colNo); 
    readChar(); 
    return token;
  case CHAR_COLON: // g???p d???u ???:??? 
    ln = lineNo; 
    cn = colNo; 
    readChar(); // ki???m tra k?? t??? ti???p theo l?? d???u ???=??? 
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) { 
      readChar(); 
      return makeToken(SB_ASSIGN, ln, cn); // tr??? v??? token l?? d???u ???:=??? 
  } else return makeToken(SB_COLON, ln, cn); // token l?? d???u ???:???
  case CHAR_SINGLEQUOTE: // g???p d???u '''
    return readConstChar();
  case CHAR_DOUBLEQUOTE: // g???p d???u '"'
    return readConstString(); 
  case CHAR_LPAR: // g???p d???u ???(??? 
    ln = lineNo; 
    cn = colNo; readChar(); 
  if (currentChar == EOF) // k???t th??c file, tr??? l???i token ???(??? 
    return makeToken(SB_LPAR, ln, cn); //n???u ch??a k???t th??c file, ki???m tra k?? t??? ti???p theo 
  switch (charCodes[currentChar]) { 
    case CHAR_PERIOD: // g???p d???u ???.??? 
    readChar(); 
    return makeToken(SB_LSEL, ln, cn); // token tr??? v??? l?? ???(.??? 
    case CHAR_TIMES: // g??p d???u ???*???, k?? t??? l?? ???(*??? ??? ti???p theo l?? comment 
      readChar(); 
      skipComment(); // b??? qua comment return getToken(); // ?????c token ti???p theo 
    default: return makeToken(SB_LPAR, ln, cn); // tr??? l???i token l?? ???(??? 
    }
  case CHAR_RPAR: // g???p d???u ???)??? 
    token = makeToken(SB_RPAR, lineNo, colNo); 
    readChar(); 
    return token; 
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;
  case TK_STRING: printf("TK_STRING(\'%s\')\n", token->string); break;
  case TK_FLOAT: printf("TK_FLOAT(%s)\n", token->string); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;
  case KW_STRING: printf("KW_STRING\n"); break;
  case KW_FLOAT: printf("KW_FLOAT\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;
  
  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }
  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }


  if (scan(argv[1]) == IO_ERROR) {
    
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}