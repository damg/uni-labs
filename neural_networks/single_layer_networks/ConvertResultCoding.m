function Y = ConvertResultCoding(sln, z)
  e = eye(sln.cOut);
  Y = e(:,z);
end