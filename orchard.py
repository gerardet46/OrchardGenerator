"""Orchard network and polynomial."""
from sympy import symbols, poly
from sympy.polys.polytools import Poly
from cached_property import cached_property
import IMLN


class OrchardNetwork(IMLN.IMLN): # noqa
    def __init__(self, pol:Poly =None, sequence:list =None, **kwargs):  # noqa
        """
        Same as IMLN, but now supossing is orchard.

        * pol: obtain from g1 polynomial.
        * sequence: obtain from a sequence.
        """
        self._lenX = 0
        if pol is not None:
            p = OrchardPolynomial(len(pol.gens), pol)
            sequence = p.smallest_CPS

        if sequence is not None:
            kwargs["eNewick"] = None
            super().__init__(**kwargs)
            self.from_sequence(sequence)
            self.auto_labelling()
        else:
            super().__init__(**kwargs)

    def poly(self, N=None): # noqa
        """
        Return the g polynomial of this network, with N leaves
        (None for automatic).
        """
        if N is None:
            N = len(self.leaves)
        return OrchardPolynomial(N, self.pol_g1())

    @property
    def lenX(self):
        """Return the maximum leaf number."""
        return self._lenX


    def from_sequence(self, seq): # noqa
        if len(seq) == 0:
            raise Exception("Sequence length must be >= 1")

        if len(self.nodes) > 0:
            self.clear()

        self.sequence = seq
        for pair in reversed(seq):
            self.add_pair(pair)


    def reduce_pair(self, pair): # noqa
        """Reduces pair=(i,j)"""
        if pair not in self.reducible_pairs:
            raise Exception("Not a reducible pair")

        p = (str(pair[0]), str(pair[1]))

        l1 = self.node_with_label(p[0])
        p1 = self.parent(l1)
        l2 = self.node_with_label(p[1])
        p2 = self.parent(l2)

        if self.is_reticulation(p1):
            # reduce reticulated cherry
            self.remove_edge(p2, p1)
            self.remove_node_and_reconnect(p1)
            self.remove_node_and_reconnect(p2)
            pass
        else:
            # reduce cherry
            self.remove_node_and_reconnect(l1)
            self.remove_node_and_reconnect(p2)
            pass

        self.clear_cache()


    def add_pair(self, pair): # noqa
        """Adds pair=(i,j) to N"""
        if len(self.nodes) == 0:
            # start with only a cherry
            root, u1, u2 = self.new_node(), self.new_node(), self.new_node()

            self.root = root
            self.add_edge(root, u1)
            self.add_edge(root, u2)

            self.nodes[u1]['label'] = str(pair[0])
            self.nodes[u2]['label'] = str(pair[1])
        else:
            if str(pair[1]) not in self.taxa:
                raise Exception(f"Second coordinate of pair ({pair[0]},{pair[1]}) not in taxa")
            if str(pair[0]) in self.taxa:
                # reticulated
                l1 = self.node_with_label(str(pair[0]))
                l2 = self.node_with_label(str(pair[1]))
                p1 = self.parent(l1)
                p2 = self.parent(l2)
                t = self.new_node()
                r = self.new_reticulation_node()

                self.remove_edge(p1, l1)
                self.remove_edge(p2, l2)

                self.add_edge(p1, r)
                self.add_edge(r, l1)

                self.add_edge(p2, t)
                self.add_edge(t, l2)
                self.add_edge(t, r)
            else:
                # cherry
                l1 = self.node_with_label(str(pair[1]))
                p = self.parent(l1)
                t = self.new_node()
                l2 = self.new_node()

                self.nodes[l2]['label'] = str(pair[0])
                self.remove_edge(p, l1)

                self.add_edge(p, t)
                self.add_edge(t, l1)
                self.add_edge(t, l2)

            if int(pair[0]) > self._lenX:
                self._lenX = int(pair[0])
            if int(pair[1]) > self._lenX:
                self._lenX = int(pair[1])
            self.auto_labelling()

    # Operations for orchard that are already implemented in OrchardPolynomial
    @cached_property # noqa
    def reducible_pairs(self):
        """Return the reducible pairs."""
        return self.poly(self._lenX).reducible_pairs

    @cached_property # noqa
    def smallest_pair(self):
        """Return the reducible pairs."""
        return self.poly(self._lenX).smallest_pair

    @cached_property # noqa
    def smallest_CPS(self):
        """Return the reducible pairs."""
        return self.poly(self._lenX).smallest_CPS

    @cached_property # noqa
    def all_CPS(self):
        """Return the reducible pairs."""
        return self.poly(self._lenX).all_CPS




class OrchardPolynomial(): # noqa
    def __init__(self, N: int = 100, pol=None):
        """
        Class with orchard operations for a polynomial.

        * N: the number of leaves (1000 if no value is given)
        * pol: set this initial pol (it must be orchard)
        """
        gens = [symbols("lambda")] + [symbols(f"x_{i}") for i in range(1, N+1)]
        self._N = N
        self._gens = tuple(gens)
        self._pol = poly(0, gens=self._gens) if pol is None else poly(pol, gens=self._gens)
        self._leaves = []

        # detect leaves in polynomial
        if pol is not None:
            for m in pol.monoms():
                for i in range(1, len(m)):
                    if m[i] > 0 and i not in self._leaves:
                        self._leaves.append(i)


    def clear_cache(self): # noqa
        """Clears the cache of all computed properties"""
        names = dir(self.__class__)
        for name in names:
            attr = getattr(self.__class__, name)
            if attr.__class__ is cached_property:
                try:
                    delattr(self, name)
                except:
                    pass


    @cached_property # noqa
    def pol(self): # noqa
        """Return the actual polynomial"""
        return self._pol

    @cached_property
    def leaves(self): # noqa
        """Return the variables used"""
        return self._leaves

    @cached_property # noqa
    def N(self):
        """Return the original N."""
        return self._N

    def _cherry_tuple(self, pair: tuple): # noqa
        """Return tuple for the cherry monomial."""
        p = (int(pair[0]), int(pair[1]))
        ls = [0] * (self._N + 1)
        ls[p[0]] = 1
        ls[p[1]] = 1
        return tuple(ls)

    def _ret_cherry_tuple(self, pair: tuple): # noqa
        """Return tuple for the reticulated cherry monomial."""
        p = (int(pair[0]), int(pair[1]))
        ls = [0] * (self._N + 1)
        ls[p[0]] = 1
        ls[p[1]] = 1
        ls[0] = 1
        return tuple(ls)


    def from_sequence(self, sequence): # noqa
        """Generates it from sequence"""
        self._pol = poly(0, gens=self._gens)
        self._leaves = []
        for s in reversed(sequence):
            self.add_pair(s)


    def add_pair(self, pair: tuple): # noqa
        """Adds a addable pair."""
        p = (int(pair[0]), int(pair[1]))

        if not self._pol.is_zero and p[1] not in self._leaves:
            raise Exception("Invalid pair")

        new_dict = {}
        if p[0] in self._leaves:
            # reticulated cherry
            new_dict[self._ret_cherry_tuple(p)] = 1
            if not self.pol.is_zero:
                for m in self.pol.monoms():
                    new = list(m)
                    new[0] += new[p[0]] + new[p[1]]
                    new[p[0]] += new[p[1]]
                    new = tuple(new)
                    if new in new_dict:
                        new_dict[new] += 1
                    else:
                        new_dict[new] = 1
        else:
            # cherry
            new_dict[self._cherry_tuple(p)] = 1
            self._leaves.append(p[0])
            if not self.pol.is_zero:
                for m in self.pol.monoms():
                    new = list(m)
                    new[p[0]] = new[p[1]]
                    new = tuple(new)
                    if new in new_dict:
                        new_dict[new] += 1
                    else:
                        new_dict[new] = 1
            else:
                self._leaves.append(p[1])

        self._pol = Poly.from_dict(new_dict, gens=self._gens)
        self.clear_cache()


    def reduce_pair(self, pair: tuple): # noqa
        """Reduces a reducible pair."""
        if pair not in self.reducible_pairs:
            raise Exception("Not a reducible pair")

        p = (int(pair[0]), int(pair[1]))
        new_dict = {}
        if self._cherry_tuple(pair) in self._pol.monoms():
            # cherry
            self._leaves.remove(p[0])
            for m in self._pol.monoms():
                new = list(m)
                new[p[0]] = 0
                new = tuple(new)
                if sum(new) > 1:  # add monomial if it isn't that pair
                    if new in new_dict:
                        new_dict[new] += 1
                    else:
                        new_dict[new] = 1
        else:
            for m in self._pol.monoms():
                new = list(m)
                new[0] -= new[p[0]]
                new[p[0]] -= new[p[1]]
                new = tuple(new)
                if sum(new) > 1:  # add monomial if it isn't that pair
                    if new in new_dict:
                        new_dict[new] += 1
                    else:
                        new_dict[new] = 1

        if new_dict:
            self._pol = Poly.from_dict(new_dict, gens=self._gens)
        else:
            self._leaves.remove(p[1])
            self._pol = poly(0, gens=self._gens)
        self.clear_cache()


    @cached_property # noqa
    def is_trivial(self):
        """Check if it's the trivial polynomial."""
        return self._pol.is_zero

    @cached_property
    def root(self):
        """Return the monomial of the root."""
        return self._pol.LM()


    def exp(self, leaf: int): # noqa
        """Returns #leaf."""
        return self.root[leaf]


    @property # noqa
    def reducible_pairs(self):
        """Get a list of the reducible pairs in g."""
        if self._pol.is_zero:
            return []

        for m in self._pol.monoms():
            s = sum(m)
            if s == 2:
                # is a cherry
                yield tuple([i for i in range(1, self._N + 1) if m[i] == 1])
            elif s == 3 and m[0] == 1:
                # ret cherry
                x = tuple([i for i in range(1, self._N + 1) if m[i] == 1])
                if self.exp(x[0]) < self.exp(x[1]):
                    x = (x[1], x[0])

                yield x


    @cached_property # noqa
    def smallest_pair(self): # noqa
        """Returns the smallest reducible pair."""
        return min(self.reducible_pairs)

    @cached_property # noqa
    def smallest_CPS(self):
        """Return the smallest CPS for this polynomial."""
        copy = self.copy()
        seq = []
        while not copy.is_trivial:
            sm = copy.smallest_pair
            seq.append(sm)
            copy.reduce_pair(sm)

        return seq

    @cached_property
    def all_CPS(self): # noqa
        """Returns a list of all the CPS in this polynomial."""
        def reduce_recursive(obj): # noqa
            """Returns all CPS for the polynomial obj."""
            if obj.is_trivial:
                return [[]]

            red = []
            for s in obj.reducible_pairs:
                copy = obj.copy()
                copy.reduce_pair(s)
                for x in reduce_recursive(copy):
                    red.append([s] + x)
            return red

        return reduce_recursive(self)

    def copy(self): # noqa
        return OrchardPolynomial(self._N, self._pol)

    def __str__(self):
        """Return the actual polynomial."""
        return str(self._pol.as_expr())
