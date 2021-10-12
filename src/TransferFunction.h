#ifndef TRANSFERFUNCTION_H
#define TRANSFERFUNCTION_H

#include <QObject>

class TransferFunction : public QObject
{
    Q_OBJECT
public:
    explicit TransferFunction(QObject *parent = 0);

    virtual double map (double input) = 0;
    virtual int name () = 0;
    
signals:
    
public slots:
    
};


//! Freescale MPXA6400A
class TransferFunction400kpa : public TransferFunction
{
    /** input 0-255, out kpa
      * transfer function P = Vout/0.012105 + 3.477902
      **/
    double map ( double input ) { return ((5.0 * (input/255.0)) / 0.012105 + 3.477902); }
    int name () { return 400; }
};

//! Bosch 200kpa
class TransferFunction200kpa : public TransferFunction
{
    /** input 0-255, out kpa
      **/
    double map ( double input ) { return ((5.0 * (input/255.0)) + 0.25) / 0.0252778; }
    int name () { return 200; }
};

//! Freescale MPX4250A 250kpa
class TransferFunction250kpa : public TransferFunction
{
    /** input 0-255, out kpa
      **/
    double map ( double input ) { return ((5.0 * (input/255.0)) + 0.2) / 0.02; }
    int name () { return 250; }
};

#if defined (DIGIFANTVANAPP)
//! use 100kpa selection for AFM: input 0-255, output to volts
class TransferFunction100kpa : public TransferFunction
{
    /** input 0-255, out kpa
      **/
    double map ( double input ) { return (5.0 * (input/255)); }
    int name () { return 100; }
};
#else
//! Freescale MPXA6115A 100kpa
class TransferFunction100kpa : public TransferFunction
{
    /** input 0-255, out kpa
      **/
    double map ( double input ) { return ((5.0 * (input/255.0)) + 0.475) / 0.045; }
    int name () { return 100; }
};
#endif

//! Freescale 300kpa
class TransferFunction300kpa : public TransferFunction
{
    /** input 0-255, out kpa
      **/
    double map ( double input ) { return ((5.0 * (input/255.0)) + 0.1765) / 0.0159; }
    int name () { return 300; }
};

/** wideband lambda */
class TransferFunctionWbLambdaTechEdge : public TransferFunction {
    double map ( double input ) { double l = ( (5.0*( ( input) /1023)) * 2 + 9 ) / 14.7; return static_cast<double>(static_cast<int>(l*100+0.5))/100.0; }
    int name () { return -1; }
};

class TransferFunctionWbLambdaPlx : public TransferFunction {
    double map ( double input ) { double l = ( (5.0*( ( input) /1023)) * 2 + 10 ) / 14.7; return static_cast<double>(static_cast<int>(l*100+0.5))/100.0; }
    int name () { return -1; }
};

class TransferFunctionWbLambdaInnovateLC1 : public TransferFunction {
    double map ( double input ) { double l =  ( (5.0*( (input) /1023)) * 3 + 7.35 ) / 14.7;  return static_cast<double>(static_cast<int>(l*100+0.5))/100.0; }
    int name () { return -1; }
};
class TransferFunctionWbLambdaInnovateLM1 : public TransferFunction {
    double map ( double input ) { double l =  ( (5.0*( (input) /1023)) * 2 + 10.0 ) / 14.7;  return static_cast<double>(static_cast<int>(l*100+0.5))/100.0; }
    int name () { return -1; }
};
class TransferFunctionWbLambdaZeitronix : public TransferFunction {
    double map ( double input ) { double l = ( (5.0*( (input) /1023)) * 2 + 9.6 ) / 14.7;  return static_cast<double>(static_cast<int>(l*100+0.5))/100.0;}
    int name () { return -1; }
};
class TransferFunctionWbLambdaDummy : public TransferFunction {
    double map ( double input ) { return input; }
    int name () { return -99; }
};
class TransferFunctionWbLambdaRawDebug : public TransferFunction {
    double map ( double input ) { return input; }
    int name () { return -98; }
};
class TransferFunctionWbLambdaBreitbandLambdaDe40 : public TransferFunction {
    double map ( double input ) { double l = (5.0*( ( input) /1023)) * 0.12 + 0.7; return static_cast<double>(static_cast<int>(l*100+0.5))/100.0; }
    int name () { return -1; }
};
//!
//! \brief The TransferFunctionWbLambdaAEM for AEM X-Series P/N 30-0300 https://www.aemelectronics.com/files/instructions/30-0300.pdf
//!
class TransferFunctionWbLambdaAEMXSeries : public TransferFunction {
    double map ( double input ) { double l = (5.0*( ( input) /1023)) * 0.1621 + 0.499; return l; }
    int name () { return -1; }
};

#endif // TRANSFERFUNCTION_H
