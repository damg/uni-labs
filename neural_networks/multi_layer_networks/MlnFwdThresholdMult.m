function [Y,Z] = MlnFwdThresholdMult(mln, X)
  [Y, Z] = MlnFwdLinear(mln, X);
  Y = ConvertResultCoding(mln, DscMax(Y));
end