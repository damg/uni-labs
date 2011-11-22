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

%% SlnDeltaTrain

%% Author: Dmitri Bachtin <dima2001@MS-7673>

function [ ret ] = SlnDeltaTrain (sln, X, T, eta, maxIter, maxErrorRate)
    currentErrorRate = inf;
    currentIteration = 0;
    [Xheight, Xwidth] = size(X);
    
    while (currentIteration < maxIter)
        if mod(currentIteration, 10) == 0 && currentIteration > 0
            figure(currentIteration)
            SlnPlotTwoClasses(sln, X, T)
        end
        nety = SlnApplyMany(sln, X);
        currentErrorRate = SlnErrorRate(nety, T);
        
        % break on epic success
        if currentErrorRate < maxErrorRate
            break
        end
        
        dW = eta * (T - nety) * X';
        sln.W1 = sln.W1 + dW;
        db = eta * (T - nety) * ones(Xwidth, 1); % 1 = Eingabe von Bias
        sln.b1 = sln.b1 + db;
        currentIteration = currentIteration + 1;
    end
    
    ret.currentErrorRate = currentErrorRate;
    ret.currentIteration = currentIteration;
    ret.sln = sln;
end