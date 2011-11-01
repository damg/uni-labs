## -*- mode: octave -*-

## Copyright (C) 2011 Dmitri Bachtin
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

## SlnInitAnd

## Author: Dmitri Bachtin <dima2001@MS-7673>
## Created: 2011-11-01

function [ ret ] = SlnInitAnd ()
  ret = SlnInit(2, 1, @SlnActFnThreshold);
  ret.W1 = [1 1];
  ret.b1 = -2;
endfunction
