function [y x]=UnknownReality(numValues)
  x = linspace(0,1,numValues); %X-Werte anlegen
%  y = 4*sin(4*pi*x).*((x-0.5).^3+(x-0.5).^2); %Funktion berechnen
  y = 10*(4*sin(pi*x).*((0.5.*x-0.5).^3)-0.25.*x); %Funktion berechnen
end