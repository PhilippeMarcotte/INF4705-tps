

################################################################################
#
# Ce script vérifie si votre solution est valide. C'est le script que nous
# utiliserons pour la correction, donc assurez-vous que la sortie de votre
# script tp.sh est compatible avec ce script-ci.
#
# Argument 1 : Path vers l'exemplaire
# Argument 2 : Path vers la solution de cet exemplaire
#
# Exemple d'utilisation :
#
#   1. Vous exécutez votre algorithme avec tp.sh et vous envoyez son résultat
#      vers un fichier texte :
#
#      ./tp.sh -e /home/pholi/INF4705/TP3/b200_100000_7 -p > sol_ex_7.txt
#
#   2. Vous vérifiez si votre solution est valide avec ce script-ci :
#
#      python3 sol_check.py /home/pholi/INF4705/TP3/b200_100000_7 sol_ex_7.txt
#
#
# Contactez-moi en cas de problème (philippe.olivier@polymtl.ca).
#
################################################################################


import pathlib
import sys


# Initial sanity checks
if (len(sys.argv) != 3):
    exit("ERREUR : Ce script de vérification de solution prend deux " + \
         "arguments en entrée. Voir le code source pour un exemple.")
if (not pathlib.Path(sys.argv[1]).is_file()):
    exit("ERREUR : Fichier " + sys.argv[1] + " inexistant.")
if (not pathlib.Path(sys.argv[2]).is_file()):
    exit("ERREUR : Fichier " + sys.argv[2] + " inexistant.")

    
## PARSE INSTANCE
raw_instance = open(sys.argv[1]).readlines()

# Remove newlines
for i in range(len(raw_instance)):
    raw_instance[i] = raw_instance[i][:-1]
    
# Extract the number of boxes and the maximum stack height
num_boxes = raw_instance[0]
max_stack_height = raw_instance[1]

# Additional sanity checks
try:
    num_boxes = int(num_boxes)
    max_stack_height = int(max_stack_height)
except ValueError:
    exit("ERREUR : Problème avec l'exemplaire " + sys.argv[1] + ". Assurez-" + \
         "vous d'utiliser l'exemplaire original du site Moodle.")

# Discard useless values, clean instance, and additional sanity checks
raw_instance = raw_instance[2:]
instance = []
for i in raw_instance:
    tuple_of_str = i.split()
    if (len(tuple_of_str) != 3):
        exit("ERREUR : Problème avec l'exemplaire " + sys.argv[1] + \
             ". Assurez-vous d'utiliser l'exemplaire original du site Moodle.")
    tuple_of_int = []
    for j in tuple_of_str:
        tuple_of_int.append(int(j))
    instance.append(tuple_of_int)
if (len(instance) != num_boxes):
    exit("ERREUR : Problème avec l'exemplaire " + sys.argv[1] + ". Assurez-" + \
         "vous d'utiliser l'exemplaire original du site Moodle.")

    
## PARSE SOLUTION
raw_solution = open(sys.argv[2]).readlines()

# Remove newlines
for i in range(len(raw_solution)):
    raw_solution[i] = raw_solution[i][:-1]

# Isolate and clean the latest solution, and additional sanity checks
cutoff = 0
for i in reversed(range(len(raw_solution)-1)):
    if (raw_solution[i] == "fin"):
        cutoff = i+1
        break
raw_solution = raw_solution[cutoff:-1]
solution = []
for i in range(len(raw_solution)):
    if (len(raw_solution[i].split()) == 1):
        stack = []
        for j in range(int(raw_solution[i])):
            tuple_of_str = raw_solution[i+1+j].split()
            if (len(tuple_of_str) != 3):
                exit("ERREUR : Problème avec la solution. Les boîtes doivent " \
                     "être représentées par 3 nombres.")
            tuple_of_int = []
            for k in tuple_of_str:
                tuple_of_int.append(int(k))
            if (tuple_of_int[0] < 0):
                exit("ERREUR : Un de vos blocs a une hauteur négative.")
            stack.append(tuple_of_int)
        stack_height = 0
        for j in stack:
            stack_height += j[0]
        if (stack_height > max_stack_height):
            print("ERREUR : La tour suivante est trop haute :")
            for j in stack:
                print(j)
            exit()
        for j in range(len(stack)-1):
            for k in range(j+1, len(stack)):
                if (not(((stack[k][1] <= stack[j][1]) and \
                         (stack[k][2] <= stack[j][2])) or \
                        ((stack[j][1] <= stack[k][1]) and \
                         (stack[j][2] <= stack[k][2])))):
                    print("ERREUR : Les blocs suivants sont en conflit dans " + \
                          "une de vos tours :")
                    print(stack[k])
                    print(stack[j])
                    exit()
        solution.append(stack)

# Make sure that all boxes are used
for i in solution:
    for j in i:
        try:
            instance.remove(j)
        except ValueError:
            print("ERREUR : Le bloc", j, "de votre solution n'existe pas " + \
                  "dans l'instance originale.")
            exit()
if (len(instance) > 0):
    print("ERREUR : Votre solution n'utilise pas tous les blocs. Il manque " + \
          "les blocs suivants :")
    for i in instance:
        print(i)
    exit()
        
print("La solution de", len(solution), "tours est valide.")
