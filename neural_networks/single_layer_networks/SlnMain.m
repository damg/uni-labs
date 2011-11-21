%% -*- mode: octave -*-

%% Copyright (C) 2011 Dmitri Bachtin, Sven Hartlieb
%% 
%% This program is free software; you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation; either version 2 of the License, or
%% (at your option) any later version.
%% 
%% This program is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%% GNU General Public License for more details.
%% 
%% You should have received a copy of the GNU General Public License
%% along with Octave; see the file COPYING.  If not, see
%% <http://www.gnu.org/licenses/>.

%% SlnMain

%% Author: Dmitri Bachtin <dima2001@MS-7673>
%% Created: 2011-11-01

function [ ret ] = SlnMain ()
  clc;
  clear all;
  randn('state', 17);

  x = SlnGenerateTrainDataForAnd();
  trainingDataIn = x.dataIn;
  trainingDataOut = x.dataOut;

  net1 = SlnInit(2, 1, @SlnActFnLinear);
  net2 = SlnCopy(net1);
  net2.actFn = @SlnActFnThreshold;

  %nety1 = SlnApplyMany(net1, trainingDataIn)
  %nety2 = SlnApplyMany(net2, trainingDataIn)

  %errorRate1 = SlnErrorRate(nety1, trainingDataOut)
  %errorRate2 = SlnErrorRate(nety2, trainingDataOut)
  
  eta = 0.2
  %net1
  %ret1 = SlnDeltaTrain(net1, trainingDataIn, trainingDataOut, eta, 1000, 0.05)
  %SlnPlotTwoClasses(ret1.sln,trainingDataIn,trainingDataOut);
  %ret1.sln
  
  net2
  ret2 = SlnDeltaTrain(net2, trainingDataIn, trainingDataOut, eta, 10, 0.05)
  SlnPlotTwoClasses(ret2.sln,trainingDataIn,trainingDataOut);
  ret2.sln
  
  %%%%
  
  %% IRIS Data
  
  %netIris = SlnInit(4, 2, @SlnActFnThreshold);
  %trainingData = csvread('iris.data');
  %trainingDataIn = trainingData(:,1:4);
  %trainingDataOut = trainingData(:, 5);
  %ret3 = SlnDeltaTrain(netIris, trainingDataIn, trainingDataOut, eta, 100, 0.05);
  %ret3.sln
  ret = 'BYE';
end
