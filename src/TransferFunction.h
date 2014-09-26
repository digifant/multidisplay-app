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


//! Freescale MPXA6115A 100kpa
class TransferFunction100kpa : public TransferFunction
{
    /** input 0-255, out kpa
      **/
    double map ( double input ) { return ((5.0 * (input/255.0)) + 0.475) / 0.045; }
    int name () { return 100; }
};

//! Freescale 300kpa
class TransferFunction300kpa : public TransferFunction
{
    /** input 0-255, out kpa
      **/
    double map ( double input ) { return ((5.0 * (input/255.0)) + 0.1765) / 0.0159; }
    int name () { return 300; }
};


#endif // TRANSFERFUNCTION_H
