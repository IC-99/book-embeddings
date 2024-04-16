from itertools import permutations

class PQtreeNode:
    def __init__(self, value, children = []) -> None:
        self.value = value
        self.children = children

def get_yield(root):
    if not root:
        return []
    if not root.children:
        return [root.value]
    leaves = []
    for child in root.children:
        leaves.extend(get_yield(child))
    return leaves

def perm(root):
    if not root: return []
    if not root.children: return [[root.value]]
    # prima diramazione in base alla tipologia di nodo
    first_permutations = []
    if root.value == 'P':
        first_permutations = list(permutations(root.children))
    elif root.value == 'Q':
        first_permutations = [root.children[::], root.children[::-1]]
    res = []
    # per ogni diramazione colleziono le permutazioni che si vengono a creare
    for permutation in first_permutations:
        # inizializzo una lista in cui costruire iterativamente le permutazioni che derivano da permutation
        new_permutations = [[]]
        # scorro gli elementi della diramazione
        for element in permutation:
            # se l'elemento è un nodo speciale aggiungo a ogni permutazione in costruzione tutte le possibili permutazioni del sottoalbero radicato al nodo calcolate ricorsivamente
            if element.value == 'P' or element.value == 'Q':
                new_new_permutations = []
                for subpermutation in perm(element):
                    for r in new_permutations:
                        to_add = r[::]
                        to_add.extend(subpermutation)
                        new_new_permutations.append(to_add)
                new_permutations = new_new_permutations
            # altrimenti appendo semplicemente il valore del nodo a ogni permutazione in costruzione (derivanti da permutation)
            else:
                for r in new_permutations:
                    r.append(element.value)
        # aggiungo al risultato tutte le permutazioni che sono derivate da permutation
        res.extend(new_permutations)
    return res

def is_value_in_children(root, x):
    if not root:
        return False
    for child in root.children:
        if child.value == x:
            return True
    return False

def remove_child_by_value(root, x):
    if not root:
        return False
    for i in range(len(root.children)):
        if root.children[i].value == x:
            root.children.pop(i)
            return True
    return False

def isolate(root, x):
    if not root:
        return None
    if not root.children:
        if root.value == x:
            return root
    if is_value_in_children(root, x):
        if x.value == 'Q':
            if root.children[0].value == x or root.children[-1].value == x:
                return root
            else:
                return None
        elif x.value == 'P':
            remove_child_by_value(root, x)
            return PQtreeNode('Q', [PQtreeNode(x), root])
    else:
        t_i = None
        index_of_t_i = 0
        for i in range(len(root.children)):
            if x in get_yield(root.children[i]):
                t_i = root.children[i]
                index_of_t_i = i
                break
        t_ii = isolate(t_i, x)
        if not t_ii:
            return None
        else:
            root.children.pop(index_of_t_i)
            root.children.insert(index_of_t_i, t_ii)

            if root.value == 'P':
                root.children.pop(index_of_t_i)
                t_ii.children.append(root)
                return t_ii
            elif root.value == 'Q':
                if root.children[0] == t_ii:
                    root.children.pop(0)
                    for i in range(len(t_ii.children) - 1, -1, -1):
                        root.children.insert(0, t_ii.children[i])
                    return root
                elif root.children[-1] == t_ii:
                    root.children.pop(len(root.children) - 1)
                    for child in t_ii.children:
                        root.children.append(child)
                    return root
                else:
                    return None
                
def print_PQtree(root, depth = 1):
    if not root:
        print('---')
    if not root.children:
        print(' ' * depth, root.value)
    for child in root.children:
        print('-' * depth)
        print_PQtree(child, depth + 1)


tree1 = PQtreeNode('Q', [PQtreeNode(1),
                         PQtreeNode(3),
                         PQtreeNode('P', [PQtreeNode(2), PQtreeNode(4)]),
                         PQtreeNode(5),
                         PQtreeNode(10),
                         PQtreeNode(7),
                         PQtreeNode('P', [PQtreeNode(8), PQtreeNode(9)]),
                         PQtreeNode(6)])

tree2 = PQtreeNode('P', [PQtreeNode(1),
                         PQtreeNode(2),
                         PQtreeNode('Q', [PQtreeNode(3), PQtreeNode(4), PQtreeNode(5)])])

tree3 = PQtreeNode('P', [PQtreeNode('A'),
                         PQtreeNode('B'),
                         PQtreeNode('Q', [PQtreeNode('C'), PQtreeNode('D'), PQtreeNode('E')])])

#print(get_yield(tree1))
#print(get_yield(tree2))

print(perm(tree2))
print(perm(tree1))

print_PQtree(tree1)