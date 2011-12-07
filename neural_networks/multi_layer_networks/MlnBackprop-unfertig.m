function [deltaW1, deltaB1, deltaW2, deltaB2]=MlnBackprop(mln, X, T, Y, Z)
  N = size(X,2);
  
  deltak = Y-T;
  deltaW2 = deltak * hostnotfoudn;
  %deltaB2 = hier fehlt was!;
  
  deltaj = (1-Z.^2).*(mln.W2'*deltak);
  %deltaW1 = hier fehlt was!;
  %deltaB1 = hier fehlt was!;
end
