#!/usr/bin/env python
import sys

N = int(sys.argv[1])
r = int(sys.argv[2])
cond = sys.argv[3]  # none, sf, tc
L = N + r - 1
acc = False  # comptar les que són a X però amb longitud menor? (útil per TC)
mostrar = False  # imprimir seqüències

# (a, b, R?), R = 1 si es reticulada, 0 altrament
redpairs = [[] for i in range(L)]

X = [N]  # fulles que tenim actualment

# Xstate desa l'estat de cada fulla:
# 0 -> no està
# 2 -> sonpare és una reticulació (per sf i tc)
# 3 -> el seu germà és reticulat (per tc)
# 1 -> altrament
Xstate = [0] * N

S = []  # seqüència
count = 0  # recompte


def condNone(a, b, isret, redpairs, rind, Xstate):
    """No filtra res."""
    return True


def condSF(a, b, isret, redpairs, rind, Xstate):
    """Mira si es satisfà la condició stack-free."""
    return not isret or Xstate[a - 1] != 2


def condTC(a, b, isret, redpairs, rind, Xstate):
    """Mira si es satisfà la condició tree-child."""
    return not isret or Xstate[a - 1] < 2


conds = {
    "none": condNone,
    "tc": condTC,
    "sf": condSF,
}


def augmentar(a, b, isret, redpairs, rind, Xstate, canvis):
    """Recalculam els parells reduïbles."""
    smallest = False  # si (a, b) és el més petit dels nous
    hiha = False  # True si sobreviu almanco un parell
    gir = None  # apunta al parell que s'ha girat (si pertoca)
    pair = (a, b, int(isret))  # parell que volem afegir
    redpairs[rind] = [pair]  # si és l'smallest el primer red. pair serà el nou

    for k in range(len(redpairs[rind - 1])):
        # comparam amb el primer supervivent
        if hiha and not smallest:
            if pair < redpairs[rind][1]:
                # És smallest. Seguim per recalcular-los tots i avançar
                smallest = True
            else:
                # no cal mirar més, no serà smallest
                return False

        # miram com canvia aquest red. pair
        ai, bi, c = redpairs[rind - 1][k]

        # Cas 1: queda igual
        if ai != a and ai != b and bi != a and bi != b:
            addsafe((ai, bi, c), gir, hiha, redpairs, rind)
            hiha = True

        # Cas 2: passam de cherry a reticulated (passa com a molt un pic)
        elif isret and c == 0:
            if a == ai and b != bi:
                addsafe((ai, bi, 1), gir, hiha, redpairs, rind)
                hiha = True
                canvis.append((bi, Xstate[bi - 1]))
                Xstate[bi - 1] = 3
            elif a == bi and b != ai:
                gir = (bi, ai, 1)
                canvis.append((ai, Xstate[ai - 1]))
                Xstate[ai - 1] = 3

        # Cas 3: altrament, desapareix (no feim res)

    # miram si el gir persisteix
    if gir:
        redpairs[rind].append(gir)
        hiha = True

    return smallest or not hiha or pair < redpairs[rind][1]


def generar(redpairs, rind, S, N, L, X, Xstate):
    """Generam orchards."""
    global count

    # provam d'afegir cada parella (a,b)
    for b in X:
        for a in range(1, N + 1):
            # no poden ser iguals
            if a == b:
                continue

            # és reticulada?
            isret = Xstate[a - 1] > 0

            # mirar si estam forçats a triar cherry normal
            if isret and L - N == rind - len(X):
                continue

            # no permetem afegir una cherry (a, b) amb a > b
            if not isret and a > b:
                continue

            # miram condició desitjada
            if not conds[cond](a, b, isret, redpairs, rind, Xstate):
                continue

            # desam els canvis de Xstate per desfer-los més tard
            canvis = []
            smallest = augmentar(a, b, isret, redpairs, rind, Xstate, canvis)

            # miram si és l'smallest i seguim endavant
            if smallest:
                S.insert(0, (a, b))
                canvis.append((a, Xstate[a - 1]))
                canvis.append((b, Xstate[b - 1]))

                if isret:
                    Xstate[a - 1] = 2
                    Xstate[b - 1] = 3
                else:
                    X.insert(0, a)
                    Xstate[a - 1] = 1
                    Xstate[b - 1] = 1

                if len(S) == L:
                    count += 1
                    printseq(S)
                else:
                    if acc and len(X) == N:
                        printseq(S)
                        count += 1

                    # seguim avançant
                    generar(redpairs, rind + 1, S, N, L, X, Xstate)

                del S[0]
                if not isret:
                    del X[0]

            # desfem els canvis
            for k, v in canvis:
                Xstate[k - 1] = v


def printseq(S):
    """Mostram la seqüència."""
    if mostrar:
        print("".join([f"({a},{b})" for a, b in S]))


def addsafe(nova, gir, hiha, redpairs, rind):
    """Afegeix mirant si s'ha d'afegir el gir o no."""
    if gir and gir < nova:
        # afegim el gir i sabem que no hi haurà més
        redpairs[rind].append(gir)
        gir = None

    # afegim la que volíem afegir
    redpairs[rind].append(nova)


#### ALGORISME ####
for i in range(1, N):
    # començam amb S=(i, N)
    redpairs[0] = [(i, N, 0)]
    S = [(i, N)]
    X.insert(0, i)
    Xstate[i - 1] = 1
    Xstate[N - 1] = 1

    # Si N = 2
    if N == 2 and acc:
        printseq(S)
        count += 1

    # generam les seqüències començant per S=(i,N)
    generar(redpairs, 1, S, N, L, X, Xstate)

    # preparam la pròxima iteració
    Xstate[i - 1] = 0
    del X[0]

# resultat final
print(count)
