/*
  Mask, used em convolution's function, declaration.
  Copyright (C) 2005  Lucas Lorensi dos Santos

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef MASK_H
#define MASK_H

#include <QVector>
#include <QBitArray>
#include <iostream>
//#include <vector>

/*!
* \class Mask
* \brief Used to apply a kernel mask into an image.
**/
class Mask
{
  private:
    //std::vector< std::vector<double> > mask;
    QVector<double> data;
    int intDim;
    void equalize();
    void calcDim();

  public:
    Mask(){;}
    Mask(int Dim){ setDim(Dim);}
    Mask(int Dim, double Value);
    ~Mask(){;}

    void load(char* fileName);
    void save(char* fileName);

    unsigned colCount() { return (unsigned)dim(); }
    unsigned rowCount() { return (unsigned)dim(); }

    double& operator()(int x, int y) { return data[x*intDim+y]; }

    void setDim(int Dim);
    //std::vector<double> operator[]( int x ) { return mask[x]; }
    int dim() { return intDim; }


    void       bitArray( QBitArray BA );
    QBitArray  bitArray();
};

#endif
