"""hola."""
from sympy import symbols, poly
from sympy.polys.polytools import Poly
from cached_property import cached_property
import phylonetwork as pn


class IMLN(pn.PhylogeneticNetwork):
    """Class for IMLN. Is the same as PhylogeneticNetwork but with methods \
    prepared for polynomials."""

    def __init__(self, eNewick=None, IMLT=None, auto_labelling=True, ret_label={}, **kwargs):  # noqa
        """
        Initializes a IMLN. If `eNewick` is given, it will be populated.

        * eNewick: populate the IMLN with a eNewick expression
        * IMLT: if eNewick is None, obtain from a IMLT
        * auto_labelling: true for autolabel the reticulations
        * ret_label: dictionary of labels for the reticulations (node: "#id"),
        in case autolabelling is False
        """
        super().__init__(eNewick=eNewick, **kwargs)

        if eNewick is None and IMLT is not None:
            self.from_IMLT(IMLT)

        self.clear_cache()
        if auto_labelling:
            self.auto_labelling()
        else:
            for k in ret_label:
                self.nodes[k]['label'] = ret_label[k]

    def auto_labelling(self): # noqa
        """Autolabels all the reticulation nodes injectively."""
        for u in self.reticulations:
            self.nodes[u]['label'] = u

        self.clear_cache()


    def parent(self, u): # noqa
        """Returns one parent of u or '' if it's the root."""
        if self.root == u:
            return ''

        return list(self.predecessors(u))[0]


    def get_IMLT(self): # noqa
        """Return U(N), the IMLT for this network (actually an IMLN)."""

        def _get_IMLT(IMLT: "IMLN", u): # noqa
            """Internal use. Generate the IMLT recursively."""
            # create node
            ut = None
            if self.is_reticulation(u) or self.is_elementary(u):
                ut = IMLT.new_reticulation_node()
            else:
                ut = IMLT.new_node()

            # same label
            if self.is_labeled(u):
                IMLT.nodes[ut]['label'] = self.nodes[u]['label']

            # attach u to its children
            for v in self.neighbors(u):
                vt = _get_IMLT(IMLT, v)
                IMLT.add_edge(ut, vt)

            # return this vertex
            return ut

        # trivial case
        IMLT = IMLN()
        r = _get_IMLT(IMLT, self.root)
        IMLT.root = r
        return IMLT


    def from_IMLT(self, IMLT: "IMLN"): # noqa
        """Sets from IMLT with good labelling. It clears the existent IMLN."""

        def _from_IMLT(self: "IMLN", IMLT: "IMLN", ut, label_dict): # noqa
            """Internal use only. Recursive case."""
            # check if node already exists
            lb = IMLT.nodes[ut]['label'] if IMLT.is_labeled(ut) else "" # noqa
            if lb and lb in label_dict:
                return label_dict[lb]

            # it doesnn't exist, so create it
            u = None
            if IMLT.is_elementary(ut):
                u = self.new_reticulation_node()
            else:
                u = self.new_node()

            # set the same label
            if lb:
                label_dict[lb] = u
                self.nodes[u]['label'] = lb

            # attach to its children
            for vt in IMLT.neighbors(ut):
                v = _from_IMLT(self, IMLT, vt, label_dict)
                self.add_edge(u, v)

            return u

        # Trivial case. First delete all nodes
        self.clear()
        self.clear_cache()

        r = _from_IMLT(self, IMLT, IMLT.root, {})
        self.root = r


    @cached_property # noqa
    def symbols(self):
        """
        Return tuple of lex ordered symbols for the polynomial.

        It returns (a,b), where a is the tuple of the ordered symbols and
        b is a dict that match a name with its symbol
        """
        sy = ["lambda"]
        for u in self.labeling_dict:
            if u[0] == "#":
                sy.append(f"lambda_{self.labeling_dict[u][1:]}")
            else:
                sy.append(f"x_{self.labeling_dict[u]}")

        names = tuple(sorted(set(sy)) + ["y"])
        sym = tuple([symbols(s) for s in names])
        d = {}
        for n in names:
            d[n] = symbols(n)

        return (sym, d)


    ##### Polynomials ##### noqa
    def pol_pr(self, u=None): # noqa
        """
        Return the p_r polynomial.

        It assumes reticulations are well labelled as #l. If u
        is given, it will return p_r(u), otherwise p_r(root)
        """
        sym, sym_dict = self.symbols
        if u is None:
            u = self.root

        if self.is_leaf(u):
            return poly(f"x_{self.nodes[u]['label']}", *sym)
        elif self.is_reticulation(u) or self.is_elementary(u):
            pol = poly(sym_dict[f"lambda_{self.nodes[u]['label'][1:]}"], *sym)
            for v in self.neighbors(u):
                pol *= self.pol_pr(v)

            return pol
        else:
            pol = poly(1, *sym)
            for v in self.neighbors(u):
                pol *= self.pol_pr(v)

            return pPolynomial(poly(sym_dict['y'] + pol, *sym))


    def pol_p1(self, u=None): # noqa
        """Return the p1 polynomial."""
        return self.pol_pr().obtain_p1()

    def pol_p0(self, u=None): # noqa
        """Return the p0 polynomial."""
        return self.pol_pr().obtain_p0()


    def pol_gr(self, u=None): # noqa
        """
        Return g_r(N).

        It assumes reticulations are well labelled as #l. If u
        is given, it will return g_r(u), otherwise g_r(root)
        """
        sym, sym_dict = self.symbols
        if u is not None:
            if self.is_leaf(u):
                return gPolynomial(poly(f"x_{self.nodes[u]['label']}", *sym))
            elif self.is_reticulation(u) or self.is_elementary(u):
                pol = poly(sym_dict[f"lambda_{self.nodes[u]['label'][1:]}"], *sym)
                for v in self.neighbors(u):
                    pol *= self.pol_gr(v)

                return pol
            else:
                pol = poly(1, *sym)
                for v in self.neighbors(u):
                    pol *= self.pol_gr(v)

                return gPolynomial(pol, *sym)
        else:
            return gPolynomial(sum([self.pol_gr(v) for v in self.nodes if
                                    not self.is_elementary(v) and
                                    not self.is_leaf(v) and
                                    not self.is_reticulation(v)]), *sym)


    def pol_g1(self, u=None): # noqa
        """Return the p1 polynomial."""
        return self.pol_gr().obtain_g1()

    def pol_g0(self, u=None): # noqa
        """Return the p0 polynomial."""
        return self.pol_gr().obtain_g0()

    def pol_gru(self, u=None): # noqa
        """
        Return g_r(U(N).

        It assumes reticulations are well labelled as #l. If u
        is given, it will return p_r(u), otherwise p_r(root)
        """
        return self.get_IMLT().pol_gr()

    def pol_g1u(self, u=None): # noqa
        """Return the p1 polynomial."""
        return self.pol_gru().obtain_g1()

    def pol_g0u(self, u=None): # noqa
        """Return the p0 polynomial."""
        return self.pol_gru().obtain_g0()



class pPolynomial(Poly): # noqa
    """The p_s polynomial, a sympy polynomial with special functions"""
    @cached_property
    def lambda_gens(self):
        """Return the symbols for lambda_*."""
        return [v for v in self.gens if "lambda" in str(v)]

    @cached_property
    def x_gens(self):
        """Return the symbols for x_* (the taxa)."""
        return [v for v in self.gens if str(v)[0] == 'x']


    def obtain_p1(self): # noqa
        """Obtain p1 (1 lambda)."""
        p = self.copy()
        for u in self.lambda_gens:
            p = p.subs(u, symbols("lambda"))

        return self.__class__(p, *tuple([symbols("lambda")] + self.x_gens + [symbols("y")]))


    def obtain_p0(self): # noqa
        """Obtain p0 (no lambdas)"""
        p = self.copy()
        for u in self.lambda_gens:
            p = p.subs(u, 1)

        return self.__class__(p, *tuple(self.x_gens + [symbols("y")]))


class gPolynomial(Poly): # noqa
    """The p_s polynomial, a sympy polynomial with special functions"""
    @cached_property
    def lambda_gens(self):
        """Return the symbols for lambda_*."""
        return [v for v in self.gens if "lambda" in str(v)]

    @cached_property
    def x_gens(self):
        """Return the symbols for x_* (the taxa)."""
        return [v for v in self.gens if str(v)[0] == 'x']


    def obtain_g1(self): # noqa
        """Obtain p1 (1 lambda)."""
        g = self.copy()
        for u in self.lambda_gens:
            g = g.subs(u, symbols("lambda"))

        return self.__class__(g, *tuple([symbols("lambda")] + self.x_gens + [symbols("y")]))


    def obtain_g0(self): # noqa
        """Obtain p0 (no lambdas)"""
        g = self.copy()
        for u in self.lambda_gens:
            g = g.subs(u, 1)

        return self.__class__(g, *tuple(self.x_gens + [symbols("y")]))
