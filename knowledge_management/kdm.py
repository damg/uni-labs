from math import *

import copy

class KdmStore:
    def __init__(self, *attrs):
        self.attrs = attrs
        self.rows = []

    def __len__(self):
        return len(self.rows)

    def __getitem__(self, ix):
        if isinstance(ix, int):
            return self.rows[ix]
        elif isinstance(ix, str):
            attr_ix = self.attrs.index(ix)
            col = [ row[attr_ix] for row in self.rows ]
            return col

    def __delitem__(self, ix):
        if isinstance(ix, int):
            del self.rows[ix]
        elif isinstance(ix, str):
            attr_ix = self.attrs.index(ix)
            for e in self.rows:
                del e[attr_ix]

    def __setitem__(self, ix, row):
        if len(row) != len(self.attrs):
            raise ValueError("Row has width %d, must be %d" % (len(row), len(self.attrs)))

    def __iter__(self):
        return iter(self.rows)

    def __hash__(self):
        return (911*hash(self.attrs)) ^ (913*hash(self.rows))

    def copy(self):
        attrs=copy.copy(self.attrs)
        rows=copy.deepcopy(self.rows)
        result = KdmStore(*attrs)
        result.rows = rows
        return result

def remove_one(store, attr_ix):
    if isinstance(attr_ix, str):
        attr_ix = self.attrs.index(attr_ix)
    del store.attrs[attr_ix]
    for row in rows:
        del row[attr_ix]

def one_r(store, klass_attr):
    klasses = store[klass_attr]
    klass_set = set(klasses)
    probability_table = {}
    for attr in store.attrs:
        if attr == klass_attr:
            continue
        d = {}
        col = store[attr]

        for val in set(col): # create dicts for every possible col value
            d[val] = {}
            for k in klass_set:
                d[val][k] = 0 # create count dicts for every possible col value / every possible class combination

        for i in xrange(len(col)):
            col_val = col[i]
            klass_val = klasses[i]
            d[col_val][klass_val] += 1

        for val in d:
            max_klass = None
            max_val = None
            val_sum = 0
            for klass in d[val]:
                max_val2 = max(max_val, d[val][klass])
                val_sum += d[val][klass]
                if max_val2 != max_val:
                    max_klass = klass
                    max_val = max_val2
            d[val] = (max_klass, val_sum - max_val, val_sum)

        max_ix = -1
        max_val = None

        probability_table[attr] = d

    for attr in store.attrs:
        if attr == klass_attr:
            continue
        attr_error_rate = 0
        attr_count = 0
        for val in probability_table[attr]:
            attr_error_rate += probability_table[attr][val][1]
            attr_count += probability_table[attr][val][2]
        probability_table[attr]["##*error_rate*##"] = (attr_error_rate, attr_count)

    return probability_table

def pprint_probability_table(d):
    for attr in d:
        print "if %s" % attr
        for val in d[attr]:
            print ("\t= %s then" % val)
            for kl in d[attr][val]:
                print "\t\t%s : %d" % (kl, d[attr][val][kl])

def calc_entropy(store, klass_attr):
    col = store[klass_attr]
    counts = [float(col.count(k)) for k in set(col)]
    col_len = float(len(col))
    return -1.0 * sum([k / col_len * log(k / col_len, 2) for k in counts])

def id3(store, klass_attr):
    root_entropy = calc_entropy(store, klass_attr)
    print "Root entropy = %f" % root_entropy
    def id3_rec(store, root_entropy, depth):
        attrs = filter(lambda x: x != klass_attr, store.attrs)
        iter_store_attrs = [ (attr, klass_attr) for attr in attrs ]
        iter_stores = [ apply(KdmStore, iter_attrs) for iter_attrs in iter_store_attrs ]
        iter_store_rows = [ zip(copy.deepcopy(store[attr]), copy.deepcopy(store[klass_attr])) for attr in attrs ]
        for e in zip(iter_stores, iter_store_rows):
            e[0].rows = e[1]
        
        d = {}
        for iter_store in iter_stores:
            val_attr = iter_store.attrs[0]
            d[val_attr] = []
            for val in set(iter_store[val_attr]):
                iter_store_ = iter_store.copy()
                iter_store_.rows = filter(lambda row: row[0] == val, iter_store_.rows)
                ### TODO: add data to calculate gains properly
                ### s_v, s

                d[val_attr].append((val, calc_entropy(iter_store_, klass_attr)))

        print "Tiefe: %d, Entropien: %s" % (depth, d)

        gains = [ ( attr, root_entropy - sum(map(lambda e: 
            
    id3_rec(store, root_entropy, 0)

if __name__ == "__main__":
    store = KdmStore("textilien", "geschenkartikel", "durchschnittspreis", "klasse")
    rows=[["mittel","wenig","mittel","T"],
          ["wenig","mittel","niedrig","N"],
          ["mittel","viel","mittel","TG"],
          ["viel","wenig","hoch","TG"],
          ["wenig","mittel","hoch","TG"],
          ["viel","mittel","niedrig","TG"],
          ["wenig","viel","niedrig","G"],
          ["mittel","wenig","niedrig","N"],
          ["viel","wenig","niedrig","T"],
          ["wenig","wenig","hoch","TG"],
          ["wenig","viel","mittel","G"],
          ["viel","viel","hoch","TG"],
          ["wenig","mittel","niedrig","N"],
          ["wenig","mittel","hoch","TG"],
          ["mittel","wenig","niedrig","N"],
          ["wenig","wenig","niedrig","N"],
          ["viel","viel","mittel","TG"],
          ["viel","wenig","mittel","T"],
          ["wenig","viel","hoch","TG"],
          ["wenig","viel","mittel","G"]]
    store.rows = rows
    id3(store, "klasse")