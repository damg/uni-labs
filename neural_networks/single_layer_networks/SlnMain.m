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

  eta = 0.0001;
  errorRate = 0.05;

%% Uebung 2
  
  %nety1 = SlnApplyMany(net1, trainingDataIn)
  %nety2 = SlnApplyMany(net2, trainingDataIn)

  %errorRate1 = SlnErrorRate(nety1, trainingDataOut)
  %errorRate2 = SlnErrorRate(nety2, trainingDataOut)
  
  %net1
  %ret1 = SlnDeltaTrain(net1, trainingDataIn, trainingDataOut, eta, 1000, errorRate)
  %SlnPlotTwoClasses(ret1.sln,trainingDataIn,trainingDataOut);
  %ret1.sln
  
  %net2
  %ret2 = SlnDeltaTrain(net2, trainingDataIn, trainingDataOut, eta, 10, errorRate)
  %SlnPlotTwoClasses(ret2.sln,trainingDataIn,trainingDataOut);
  %ret2.sln
  
%% Uebung 3
  
  %% IRIS Data
  
%   netIris = SlnInit(2, 1, @SlnActFnThreshold);
%   trainingData = csvread('iris01.data');
%   
%   trainingDataIn = trainingData(:,1:2)';
%   trainingDataOut = trainingData(:, 5)';
%   
%   ret3 = SlnDeltaTrain(netIris, trainingDataIn, trainingDataOut, eta, 100, errorRate)
%   SlnPlotTwoClasses(ret3.sln, trainingDataIn, trainingDataOut);
%   ret3.sln
%   
%   validationData = csvread('iris01.data');
%   validationDataIn = validationData(:,1:2)';
%   validationDataOut = validationData(:, 5)'
%   
%   SlnApplyMany(ret3.sln, validationDataIn) == validationDataOut
  
  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
%    netIris = SlnInit(2, 1, @SlnActFnThreshold);
%    netIris.b1 = 1;
%    netIris
%    trainingData = csvread('iris12.data');
%    
%    trainingDataIn = trainingData(:,3:4)'
%    trainingDataOut = trainingData(:, 5)';
%    trainingDataOut = trainingDataOut - 1
%    
%    SlnApplyMany(netIris, trainingDataIn)
%    
%    ret4 = SlnDeltaTrain(netIris, trainingDataIn, trainingDataOut, eta, 1000, errorRate)
%    SlnPlotTwoClasses(ret4.sln, trainingDataIn, trainingDataOut);
%    ret4.sln
%    
%    validationData = csvread('validation12.data');
%    validationDataIn = validationData(:,3:4)';
%    validationDataOut = validationData(:, 5)';
%    validationDataOut = validationDataOut - 1
%    
%    SlnApplyMany(ret4.sln, validationDataIn)
  
%% Uebung 4

trainingData = csvread('iris.data.orig');
trainingDataIn = trainingData(:, 3:4);
trainingDataOut = trainingData(:, 5)' + 1; %% a.1)

netIris = SlnInit(2, 3, @SlnActFnLinear)
netIris.postProcessFn = @DscMax;

trainingDataOut2 = ConvertResultCoding(netIris, trainingDataOut); %% a.1)
ret = SlnDeltaTrain(netIris, trainingDataIn', trainingDataOut2, eta, 50000, errorRate)
ret.sln.W1
ret.sln.b1

ret = 'BYE';
end
