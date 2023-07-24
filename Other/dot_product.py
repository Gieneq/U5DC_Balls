
def get_normalized(A):
    if len(A) != 2:
        raise ValueError("Both vectors must be 2D (have 2 components)")

    A_len = (A[0]**2 + A[1]**2)**0.5
    return (A[0]/A_len, A[1]/A_len)

def dot_product_2d(A, B):
    if len(A) != 2 or len(B) != 2:
        raise ValueError("Both vectors must be 2D (have 2 components)")

    dot_product = (A[0] * B[0]) + (A[1] * B[1])
    return dot_product

if __name__ == '__main__':
    gravity = (0, -8.91)
    vectors = []
    vectors.append(((0, 1), gravity))
    vectors.append(((1, 1), gravity))
    vectors.append(((1, 0), gravity))
    vectors.append(((1, -1), gravity))
    vectors.append(((0, -1), gravity))
    vectors.append(((-1, -1), gravity))
    vectors.append(((-1, 0), gravity))
    vectors.append(((-1, 1), gravity))

    for n, a in vectors:
        nn = get_normalized(n)
        an = get_normalized(a)
        dot_res = dot_product_2d(nn, an)
        print(f"{n} o {a} -> {round(dot_res, 3)}")