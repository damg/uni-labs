function [ ret ] = DscMax (x)
    [foo, x2] = max(x);
    ret = x2;
end