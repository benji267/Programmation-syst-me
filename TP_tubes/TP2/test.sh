#!/bin/sh

PROG=${PROG:=./statoctet}		# chemin de l'exécutable

TMP=${TMP:=/tmp/test}			# chemin des logs de test

#
# Script Shell de test de l'exercice 2
# Utilisation : sh ./test.sh
#
# Si tout se passe bien, le script doit afficher "Tests ok" à la fin
# Dans le cas contraire, le nom du test échoué s'affiche.
# Les fichiers sont laissés dans /tmp/test*, vous pouvez les examiner
# Pour avoir plus de détails sur l'exécution du script, vous pouvez
# utiliser :
#	sh -x ./test.sh
# Toutes les commandes exécutées par le script sont alors affichées.
#

set -u					# erreur si accès variable non définie

# il ne faudrait jamais appeler cette fonction
# argument : message d'erreur
fail ()
{
    local msg="$1"

    echo FAIL				# aie aie aie...
    echo "$msg"
    exit 1
}

est_vide ()
{
    local fichier="$1"
    test $(wc -l < "$fichier") = 0
}

# Vérifie que le message d'erreur est envoyé sur la sortie d'erreur
# et non sur la sortie standard
# $1 = nom du fichier de log (sans .err ou .out)
verifier_stderr ()
{
    [ $# != 1 ] && fail "ERREUR SYNTAXE verifier_stderr"
    local base="$1"
    est_vide $base.err \
	&& fail "Le message d'erreur devrait être sur la sortie d'erreur"
    est_vide $base.out \
	|| fail "Rien ne devrait être affiché sur la sortie standard"
}

# Vérifie que le message d'erreur indique la bonne syntaxe
# $1 = nom du fichier de log d'erreur
verifier_usage ()
{
    [ $# != 1 ] && fail "ERREUR SYNTAXE verifier_usage"
    local err="$1"
    grep -q "usage *: statoctet *<repertoire>" $err \
	|| fail "Message d'erreur devrait indiquer 'usage:...'"
}

# Compare la sortie du programme avec ce qui est attendu
# $1 = fichier résultat
# $2 = attendu
comparer_sortie ()
{
    [ $# != 2 ] && fail "ERREUR SYNTAXE comparer_sortie"
    local out="$1" attendu="$2"

    sort $out > $TMP.sortout
    echo -n "$attendu" | sort > $TMP.sortatt

    diff $TMP.sortatt $TMP.sortout > $TMP.diff || fail "sortie invalide, cf $TMP.diff"
}

# Retourne le dernier numéro de PID attribué
lastpid ()
{
    echo > /dev/null &
    echo $!
    wait
}

# Génère le code de fausses primitives qui renvoient systématiquement
# des erreurs
# Note : on ne peut pas tester toutes les primitives

faux_read ()
{
    cat <<EOF
#include <unistd.h>
#include <errno.h>
ssize_t read(int fd, void *buf, size_t count) { errno = ELIBBAD ; return -1 ; }
EOF
}

faux_readdir ()
{
    cat <<EOF
#include <dirent.h>
#include <errno.h>
struct dirent *readdir(DIR *dirp) { errno = ELIBBAD ; return 0 ; }
EOF
}

faux_closedir ()
{
    cat <<EOF
#include <dirent.h>
#include <errno.h>
int closedir(DIR *dirp) { errno = ELIBBAD ; return -1 ; }
EOF
}

# Lance le programme avec une fausse primitive qui renvoie systématiquement -1
# $1 = primitive qui doit renvoyer -1
# $2 et suivant : le programme et ses arguments
lancer_faux ()
{
    [ $# -le 2 ] && fail "ERREUR SYNTAXE lancer_faux"
    local ps=$1

    rm -f $TMP.so
    shift
    faux_$ps | gcc -shared -fPIC -o $TMP.so -x c -
    LD_PRELOAD=$TMP.so $@
}

# Le nettoyage façon karscher : il ne reste plus une trace après...
nettoyer ()
{
    chmod -R +rx $TMP.d* 2> /dev/null
    rm -rf $TMP.*
}

##############################################################################
# Tests d'erreur sur les arguments

nettoyer

echo -n "Test 1.1 - pas assez d'arguments.................................... "
$PROG /    > $TMP.out 2> $TMP.err	&& fail "pas assez d'arg"
verifier_stderr $TMP
verifier_usage $TMP.err
echo OK

echo -n "Test 1.2 - répertoire inexistant.................................... "
rm -f $TMP.nonexistant
$PROG $TMP.nonexistant 65 > $TMP.out 2> $TMP.err && fail "répertoire inexistant"
verifier_stderr $TMP
echo OK

echo -n "Test 1.3 - argument pas un répertoire............................... "
touch $TMP.freg
$PROG $TMP.freg > $TMP.out 2> $TMP.err	&& fail "argument non répertoire"
verifier_stderr $TMP
echo OK

echo -n "Test 1.4 - octets invalides......................................... "
nettoyer
mkdir $TMP.d
touch $TMP.d/f
$PROG $TMP.freg -1 > $TMP.out 2> $TMP.err && fail "octet invalide (-1)"
verifier_stderr $TMP
$PROG $TMP.freg 256 > $TMP.out 2> $TMP.err && fail "octet invalide (256)"
verifier_stderr $TMP
echo OK

##############################################################################
# Tests basiques

echo -n "Test 2.1 - un seul octet............................................ "
nettoyer
mkdir $TMP.d
echo -n AAAAAAAAAA > $TMP.d/A
$PROG $TMP.d 65 > $TMP.out 2> $TMP.err	|| fail "sortie en erreur"
est_vide $TMP.err			|| fail "sortie d'erreur non vide"
ATTENDU="$TMP.d/A 65 10 1.000000
"
comparer_sortie $TMP.out "$ATTENDU"
echo OK

echo -n "Test 2.2 - plusieurs fichiers....................................... "
echo -n AAAAABBBBB > $TMP.d/B
$PROG $TMP.d 65 > $TMP.out 2> $TMP.err	|| fail "sortie en erreur"
est_vide $TMP.err			|| fail "sortie d'erreur non vide"
ATTENDU="$TMP.d/A 65 10 1.000000
$TMP.d/B 65 5 0.500000
"
comparer_sortie $TMP.out "$ATTENDU"
echo OK

echo -n "Test 2.3 - plusieurs fichiers et plusieurs octets................... "
echo -n BBBBBCCCCC > $TMP.d/C
$PROG $TMP.d 65 66 67 > $TMP.out 2> $TMP.err || fail "sortie en erreur"
est_vide $TMP.err			|| fail "sortie d'erreur non vide"
ATTENDU="$TMP.d/A 65 10 1.000000
$TMP.d/A 66 0 0.000000
$TMP.d/A 67 0 0.000000
$TMP.d/B 65 5 0.500000
$TMP.d/B 66 5 0.500000
$TMP.d/B 67 0 0.000000
$TMP.d/C 65 0 0.000000
$TMP.d/C 66 5 0.500000
$TMP.d/C 67 5 0.500000
"
comparer_sortie $TMP.out "$ATTENDU"
echo OK

echo -n "Test 2.4 - fichiers avec caractères non affichables................. "
# on a 4 occurrences de l'octet 0 sur 4*32 = 128 octets, soit 3,125 %
awk 'END { for (i=0;i<4;i++) for (j=0;j<32;j++) printf "%c", j}' /dev/null \
	> $TMP.d/X
$PROG $TMP.d 0 1 > $TMP.out 2> $TMP.err || fail "sortie en erreur"
est_vide $TMP.err			|| fail "sortie d'erreur non vide"
ATTENDU="$TMP.d/A 0 0 0.000000
$TMP.d/A 1 0 0.000000
$TMP.d/B 0 0 0.000000
$TMP.d/B 1 0 0.000000
$TMP.d/C 0 0 0.000000
$TMP.d/C 1 0 0.000000
$TMP.d/X 0 4 0.031250
$TMP.d/X 1 4 0.031250
"
comparer_sortie $TMP.out "$ATTENDU"
echo OK

echo -n "Test 2.5 - plein de processus....................................... "
nettoyer
mkdir $TMP.d
dd if=/dev/urandom bs=8191 count=1 of=$TMP.d/U 2> /dev/null
dd if=/dev/urandom bs=8193 count=1 of=$TMP.d/V 2> /dev/null
OCTETS=""
for i in 1 2 3 4
do
    OCTETS="$OCTETS $(seq 0 255)"
done
pid1=$(lastpid)
$PROG $TMP.d $OCTETS > $TMP.out 2> $TMP.err || fail "sortie en erreur"
pid2=$(lastpid)
est_vide $TMP.err			|| fail "sortie d'erreur non vide"
# Il doit y avoir exactement 65536 = 4*16384 (2^16) octets trouvés
n=$(awk '{ s += $3 } END { print s} ' $TMP.out)
[ $n = 65536 ]				|| fail "nombre total d'octets invalide"
# il doit y avoir 4*256 + 2 = 1026 processus
# Comme il peut y avoir des processus en plus dans le système, test >=
nproc=$((pid2-pid1))
[ $nproc -ge 1026 ]			|| fail "nombre de processus invalide"
echo OK

##############################################################################
# Tests avec des pièges potentiels à prendre en compte

echo -n "Test 3.1 - pas d'exploration récursive.............................. "
nettoyer
mkdir $TMP.d
echo -n AAAAABBBBB > $TMP.d/A
mkdir $TMP.d/subdir
echo -n CCCCCDDDDD > $TMP.d/subdir/C
$PROG $TMP.d 65 67 > $TMP.out 2> $TMP.err || fail "sortie en erreur"
est_vide $TMP.err			|| fail "sortie d'erreur non vide"
ATTENDU="$TMP.d/A 65 5 0.500000
$TMP.d/A 67 0 0.000000
"
comparer_sortie $TMP.out "$ATTENDU"
echo OK

echo -n "Test 3.2 - on suit bien les liens symboliques....................... "
# on conserve l'arborescence précédente
ln -s ../A $TMP.d/subdir/L
$PROG $TMP.d/subdir 65 67 > $TMP.out 2> $TMP.err || fail "sortie en erreur"
est_vide $TMP.err			|| fail "sortie d'erreur non vide"
ATTENDU="$TMP.d/subdir/C 65 0 0.000000
$TMP.d/subdir/C 67 5 0.500000
$TMP.d/subdir/L 65 5 0.500000
$TMP.d/subdir/L 67 0 0.000000
"
comparer_sortie $TMP.out "$ATTENDU"
echo OK

echo -n "Test 3.3 - et on détecte bien les erreurs avec les liens............ "
ln -s $TMP.d/non-existant $TMP.d/subdir/L2
$PROG $TMP.d/subdir 65 67 > $TMP.out 2> $TMP.err && fail "erreur non détectée"
est_vide $TMP.err			&& fail "message pas sur stderr"
echo OK

echo -n "Test 3.4 - arborescence avec des chemins <= 128 caractères.......... "
LIMITE=128
nettoyer
mkdir $TMP.d
d=$TMP.d
l=$(echo -n $d | wc -c)
while [ $l -le $((LIMITE-12)) ]
do
    d=$d/1234567890
    l=$((l+11))
done
mkdir -p $d
n=$((LIMITE-l-1))
last=$(echo xxxxxxxxxxxxxxxx | cut -c 1-$((n-1)) )
base=$d/${last}
dd if=/dev/urandom bs=49999 count=1 of=${base}a 2> /dev/null
dd if=/dev/urandom bs=49999 count=1 of=${base}b 2> /dev/null
dd if=/dev/urandom bs=49999 count=1 of=${base}c 2> /dev/null
$PROG $d 65 66 > $TMP.out 2> $TMP.err	|| fail "sortie en erreur"
est_vide $TMP.err			|| fail "sortie d'erreur non vide"
# on ne teste que le nombre de lignes
nl=$(wc -l < $TMP.out)
[ $nl = 6 ]				|| fail "mauvais nb de lignes en sortie"
echo OK

echo -n "Test 3.5 - arborescence avec des chemins de 129 caractères.......... "
base=${base}y
dd if=/dev/urandom bs=49999 count=1 of=${base}a 2> /dev/null
dd if=/dev/urandom bs=49999 count=1 of=${base}b 2> /dev/null
dd if=/dev/urandom bs=49999 count=1 of=${base}c 2> /dev/null
$PROG $d 65 66 > $TMP.out 2> $TMP.err	&& fail "réussite avec chemin > 128 o"
est_vide $TMP.err			&& fail "pas de msg d'erreur"
echo OK

echo -n "Test 3.6 - très grand nombre de fichiers (lent)..................... "
nettoyer
mkdir $TMP.d
for i in $(seq 1 100)
do
    for j in $(seq 1 100)
    do
	touch $TMP.d/$i-$j
    done
done
$PROG $TMP.d 65 66 > $TMP.out 2> $TMP.err || fail "sortie en erreur"
est_vide $TMP.err			|| fail "sortie d'erreur non vide"
nl=$(wc -l < $TMP.out)
# 10000 de fichiers examinés deux fois
[ $nl = 20000 ]				|| fail "mauvais nb de lignes en sortie"
echo OK

##############################################################################
# Tests de prise en compte des erreurs


echo -n "Test 4.1 - erreur avec open......................................... "
nettoyer 
mkdir $TMP.d
echo -n AAAAAAAAAA > $TMP.d/A
echo -n AAAAABBBBB > $TMP.d/B
echo -n BBBBBCCCCC > $TMP.d/C
chmod 0 $TMP.d/A
$PROG $TMP.d 65 > $TMP.out 2> $TMP.err	&& fail "open : pas de test d'err ?"
est_vide $TMP.err			&& fail "sortie d'erreur vide"
chmod 644 $TMP.d/A
echo OK

echo -n "Test 4.2 - erreur avec opendir...................................... "
# on repart du répertoire précédent
chmod u-r $TMP.d
$PROG $TMP.d 65 > $TMP.out 2> $TMP.err	&& fail "opendir : pas de test d'err ?"
est_vide $TMP.err			&& fail "sortie d'erreur vide"
chmod u+r $TMP.d
echo OK

if [ "$(uname -s)" != Linux ]
then
    echo "Les tests suivants ne peuvent être exécutés que sur Linux"
    exit 1
fi

echo -n "Test 4.3 - erreur avec readdir...................................... "
lancer_faux readdir \
	$PROG $TMP.d 65 > $TMP.out 2> $TMP.err \
					&& fail "readdir : pas de test d'err ?"
est_vide $TMP.err			&& fail "sortie d'erreur vide"
echo OK

echo -n "Test 4.4 - erreur avec closedir..................................... "
lancer_faux closedir \
	$PROG $TMP.d > $TMP.out 2> $TMP.err \
					&& fail "closedir : pas de test d'err ?"
est_vide $TMP.err			&& fail "sortie d'erreur vide"
echo OK

echo -n "Test 4.5 - erreur avec read......................................... "
lancer_faux read \
	$PROG $TMP.d > $TMP.out 2> $TMP.err \
					&& fail "read : pas de test d'err ?"
est_vide $TMP.err			&& fail "sortie d'erreur vide"
echo OK

##############################################################################
# Fini !

nettoyer
echo "Tests ok"
exit 0
