function mln=MlnInit(dIn,mHidden,cOut)
  mln.dIn = dIn;
  mln.mHidden = mHidden;
  mln.cOut = cOut;
  mln.numWeights = (dIn+1)*mHidden+(mHidden+1)*cOut;
  
  randn('state', 42);
  mln.W1 = randn(mHidden, dIn)./sqrt(dIn+1);
  mln.b1 = randn(mHidden,1)./sqrt(dIn+1);
  mln.W2 = randn(cOut, mHidden)./sqrt(mHidden+1);
  mln.b2 = randn(cOut,1)./sqrt(mHidden+1);
end
