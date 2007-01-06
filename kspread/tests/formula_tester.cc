/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <klocale.h>
#include <kdebug.h>

#include "tester.h"
#include "formula_tester.h"

#include <Formula.h>
#include <Value.h>

#define CHECK_PARSE(x,y)  checkParse(__FILE__,__LINE__,#x,x,y)
#define CHECK_EVAL(x,y)  checkEval(__FILE__,__LINE__,#x,x,y)
#define CHECK_OASIS(x,y)  checkOasis(__FILE__,__LINE__,#x,x,y)

using namespace KSpread;

FormulaParserTester::FormulaParserTester(): Tester()
{
}

QString FormulaParserTester::name()
{
  return QString("Formula (Parser)");
}

static char encodeTokenType( const Token& token )
{
  char result = '?';
  switch( token.type() )
  {
    case Token::Boolean:    result = 'b'; break;
    case Token::Integer:    result = 'i'; break;
    case Token::Float:      result = 'f'; break;
    case Token::Operator:   result = 'o'; break;
    case Token::Cell:       result = 'c'; break;
    case Token::Range:      result = 'r'; break;
    case Token::Identifier: result = 'x'; break;
    default: break;
  }
  return result;
}

static QString describeTokenCodes( const QString& tokenCodes )
{
  QString result;

  if( tokenCodes.isEmpty() )
    result = "(invalid)";
  else
    for( int i = 0; i < tokenCodes.length(); i++ )
    {
      switch( tokenCodes[i].unicode() )
      {
        case 'b': result.append( "Boolean" ); break;
        case 'i': result.append( "integer" ); break;
        case 'f': result.append( "float" ); break;
        case 'o': result.append( "operator" ); break;
        case 'c': result.append( "cell" ); break;
        case 'r': result.append( "range" ); break;
        case 'x': result.append( "identifier" ); break;
        default:  result.append( "unknown" ); break;
      }
      if( i < tokenCodes.length()-1 ) result.append( ", " );
    }

  return result.prepend("{").append("}");
}

void FormulaParserTester::checkParse( const char *file, int line, const char* msg,
  const QString& formula, const QString& tokenCodes )
{
  testCount++;

  Formula f;
  QString expr = formula;
  expr.prepend( '=' );
  f.setExpression( expr );
  Tokens tokens = f.tokens();

  QString resultCodes;
  if( tokens.valid() )
    for( int i = 0; i < tokens.count(); i++ )
      resultCodes.append( encodeTokenType( tokens[i] ) );

  if( resultCodes != tokenCodes )
  {
    QString message = msg;
    message.append( " Result: ").append( describeTokenCodes( resultCodes ) );
    message.append( " Expected: ").append( describeTokenCodes( tokenCodes ) );
    fail( file, line, message );
  }
}

void FormulaParserTester::run()
{
  testCount = 0;
  errorList.clear();

  // simple, single-token formulas
  CHECK_PARSE( "True", "x" );
  CHECK_PARSE( "False", "x" );
  CHECK_PARSE( "36", "i" );
  CHECK_PARSE( "0", "i" );
  CHECK_PARSE( "3.14159", "f" );
  CHECK_PARSE( ".25", "f" );
  CHECK_PARSE( "1e-9", "f" );
  CHECK_PARSE( "2e3", "f" );
  CHECK_PARSE( ".3333e0", "f" );

  // cell/range/identifier
  CHECK_PARSE( "A1", "c" );
  CHECK_PARSE( "Sheet1!A1", "c" );
  CHECK_PARSE( "'Sheet1'!A1", "c" );
  CHECK_PARSE( "'Sheet One'!A1", "c" );
  CHECK_PARSE( "2006!A1", "c" );
  CHECK_PARSE( "2006bak!A1", "c" );
  CHECK_PARSE( "2006bak2!A1", "c" );
  CHECK_PARSE( "'2006bak2'!A1", "c" );
  CHECK_PARSE( "A1:B100", "r" );
  CHECK_PARSE( "Sheet1!A1:B100", "r" );
  CHECK_PARSE( "'Sheet One'!A1:B100", "r" );
  CHECK_PARSE( "SIN", "x" );
  // log2 and log10 are cell references and function identifiers
  CHECK_PARSE( "LOG2", "c" );
  CHECK_PARSE( "LOG10:11", "r" );
  CHECK_PARSE( "LOG2(2)", "xoio" );
  CHECK_PARSE( "LOG10(10)", "xoio" );

  // operators
  CHECK_PARSE( "+", "o" );
  CHECK_PARSE( "-", "o" );
  CHECK_PARSE( "*", "o" );
  CHECK_PARSE( "/", "o" );
  CHECK_PARSE( "+", "o" );
  CHECK_PARSE( "^", "o" );
  CHECK_PARSE( "(", "o" );
  CHECK_PARSE( ")", "o" );
  CHECK_PARSE( ",", "o" );
  CHECK_PARSE( ";", "o" );
  CHECK_PARSE( "=", "o" );
  CHECK_PARSE( "<", "o" );
  CHECK_PARSE( ">", "o" );
  CHECK_PARSE( "<=", "o" );
  CHECK_PARSE( ">=", "o" );
  CHECK_PARSE( "%", "o" );

  // commonly used formulas
  CHECK_PARSE( "A1+A2", "coc" );
  CHECK_PARSE( "2.5*B1", "foc" );
  CHECK_PARSE( "SUM(A1:Z10)", "xoro" );
  CHECK_PARSE( "MAX(Sheet1!Sales)", "xoro" );
  CHECK_PARSE( "-ABS(A1)", "oxoco" );

  // should be correctly parsed though they are nonsense (can't be evaluated)
  CHECK_PARSE( "0E0.5", "ff" );
  CHECK_PARSE( "B3 D4:D5 Sheet1!K1", "crc" );
  CHECK_PARSE( "SIN A1", "xc" );
  CHECK_PARSE( "SIN A1:A20", "xr" );

  // invalid formulas, can't be parsed correctly
  CHECK_PARSE( "+1.23E", QString::null );

#ifdef KSPREAD_INLINE_ARRAYS
  // inline arrays
  CHECK_PARSE( "{1;2|3;4}", "oioioioio" );
#endif
}

FormulaEvalTester::FormulaEvalTester(): Tester()
{
}

QString FormulaEvalTester::name()
{
  return QString("Formula (Eval)");
}

void FormulaEvalTester::checkEval( const char *file, int line, const char* msg,
  const QString& formula, const Value& expected )
{
  testCount++;

  Formula f;
  QString expr = formula;
  if ( expr[0] != '=' )
    expr.prepend( '=' );
  f.setExpression( expr );
  Value result = f.eval();

  bool equality = false;
  if ( expected.type() == Value::Array )
  {
    const uint cols = expected.columns();
    const uint rows = expected.rows();
    if ( cols == result.columns() )
    if ( rows == result.rows() )
    for ( uint row = 0; row < rows; ++row )
      for ( uint col = 0; col < cols; ++col )
        if ( !result.element(col,row).equal( expected.element(col,row) ) ) break;
    equality = true;
  }
  else
    equality = result.equal( expected );

  if( !equality )
  {
    QString message;
    QTextStream ts( &message, QIODevice::WriteOnly );
    ts << msg;
    ts << " Result: " << result;
    ts << " Expected: " << expected;
    fail( file, line, message );
  }
}


void FormulaEvalTester::run()
{
  testCount = 0;
  errorList.clear();

  // simple constants
  CHECK_EVAL( "0", Value(0) );
  CHECK_EVAL( "1", Value(1) );
  CHECK_EVAL( "-1", Value(-1) );
  CHECK_EVAL( "3.14e7", Value(3.14e7) );
  CHECK_EVAL( "3.14e-7", Value(3.14e-7) );


  // simple binary operation
  CHECK_EVAL( "0+0", Value(0) );
  CHECK_EVAL( "1+1", Value(2) );

  // unary minus
  CHECK_EVAL( "-1", Value(-1) );
  CHECK_EVAL( "--1", Value(1) );
  CHECK_EVAL( "---1", Value(-1) );
  CHECK_EVAL( "----1", Value(1) );
  CHECK_EVAL( "-----1", Value(-1) );
  CHECK_EVAL( "5-1", Value(4) );
  CHECK_EVAL( "5--1", Value(6) );
  CHECK_EVAL( "5---1", Value(4) );
  CHECK_EVAL( "5----1", Value(6) );
  CHECK_EVAL( "5-----1", Value(4) );
  CHECK_EVAL( "5-----1*2.5", Value(2.5) );
  CHECK_EVAL( "5------1*2.5", Value(7.5) );
  CHECK_EVAL( "-SIN(0)", Value(0) );
  CHECK_EVAL( "1.1-SIN(0)", Value(1.1) );
  CHECK_EVAL( "1.2--SIN(0)", Value(1.2) );
  CHECK_EVAL( "1.3---SIN(0)", Value(1.3) );
  CHECK_EVAL( "-COS(0)", Value(-1) );
  CHECK_EVAL( "1.1-COS(0)", Value(0.1) );
  CHECK_EVAL( "1.2--COS(0)", Value(2.2) );
  CHECK_EVAL( "1.3---COS(0)", Value(0.3) );

  // no parentheses, checking operator precendences
  CHECK_EVAL( "14+3*77", Value(245) );
  CHECK_EVAL( "14-3*77", Value(-217) );
  CHECK_EVAL( "26*4+81", Value(185) );
  CHECK_EVAL( "26*4-81", Value(23) );
  CHECK_EVAL( "30-45/3", Value(15) );
  CHECK_EVAL( "45+45/3", Value(60) );
  CHECK_EVAL( "4+3*2-1", Value(9) );

  // power operator is left associative
  CHECK_EVAL( "2^3", Value(8) );
  CHECK_EVAL( "2^3^2", Value(64) );

  // lead to division by zero
  CHECK_EVAL( "0/0", Value::errorDIV0() );
  CHECK_EVAL( "1/0", Value::errorDIV0() );
  CHECK_EVAL( "-4/0", Value::errorDIV0() );
  CHECK_EVAL( "(2*3)/(6-2*3)", Value::errorDIV0() );
  CHECK_EVAL( "1e3+7/0", Value::errorDIV0() );
  CHECK_EVAL( "2^(99/0)", Value::errorDIV0() );

  // string expansion ...
  CHECK_EVAL( "\"2\"+5", Value(7) );
  CHECK_EVAL( "2+\"5\"", Value(7) );
  CHECK_EVAL( "\"2\"+\"5\"", Value(7) );

  //the built-in sine function
  CHECK_EVAL ("SIN(0)", Value(0));
  CHECK_EVAL ("2+sin(\"2\"-\"2\")", Value(2));
  CHECK_EVAL ("\"1\"+sin(\"0\")", Value(1));


#ifdef KSPREAD_INLINE_ARRAYS
  // inline arrays
  Value array(2,2);
  array.setElement(0,0,Value(1));
  array.setElement(1,0,Value(2));
  array.setElement(0,1,Value(3));
  array.setElement(1,1,Value(4));
  CHECK_EVAL( "={1;2|3;4}", array );
  array.setElement(1,0,Value(0));
  CHECK_EVAL( "={1;SIN(0)|3;4}", array ); // "dynamic"
  CHECK_EVAL( "=SUM({1;2|3;4})", Value(10) );
#endif
}


