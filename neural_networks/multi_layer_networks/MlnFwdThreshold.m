function [Y,Z] = MlnFwdThreshold(mln, X)
  [Y, Z] = MlnFwdLinear(mln, X);
  Y = Y >= 0;
end