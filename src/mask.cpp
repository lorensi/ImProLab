/*
  Mask, used em convolution's function, implementation.
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

#include <stdio.h>
#include <fstream>
#include <math.h>
#include "mask.h"

using namespace std;

Mask::Mask(int Dim, double Value)
{
  setDim(Dim);

  for(int i=0; i < Dim; ++i)
    for(int j=0; j < Dim; ++j)
      data[i*Dim+j] = Value;
}
void Mask::load(char* fileName)
{
  data.clear();

  FILE *file = fopen( fileName, "r");

  if(file)
  {
    float aux;

    while( !feof(file) )
    {
      fscanf(file,"%f ",&aux);
        data.push_back(aux);
    }
    fclose(file);
  }
  calcDim();

  equalize();
}

void Mask::calcDim()
{
  intDim = (int)sqrt((double)data.size());
}

void Mask::save(char* fileName)
{
  ofstream of(fileName);

  for( int i=0; i < data.size(); ++i)
  {
    of << data[i] << " ";

    if( !((i+1) % intDim) ) of << endl;
  }

  of.close();
}

void Mask::setDim(int Dim)
{
  data.resize( Dim*Dim );
  intDim = Dim;
}

void Mask::bitArray( QBitArray BA )
{
  data.clear();
  for( int i=0; i < BA.size(); ++i)
  {
    data.push_back( (BA[i] ? 1. : 0.) );
  }
  calcDim();
}

QBitArray Mask::bitArray()
{
  QBitArray ba( data.size() );
  for( int i=0; i < data.size(); ++i)
  {
    ba[i] = (bool)data[i];
  }
  return ba;
}

void Mask::equalize()
{
  double acc = 0.0;

  for( int i=0; i < data.size(); ++i)
  {
    acc += data[i];
  }

  if( acc > 1.0 )
  {
    for( int i=0; i < data.size(); ++i)
    {
      data[i] /= acc;
    }
  }
}


