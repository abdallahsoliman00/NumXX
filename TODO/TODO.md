TODO List
---

This contains, as the name suggests, a todo list for new features / bug fixes / optimisations / etc...

## TODO:
- Add a cache memory function where the results of functions that are reused many times can be cached for reuse. (See [Cache.cpp](Cache.cpp))

- Add the FAST Fourier Transform
- Add matrix inversion and eigenvectors/eigenvalues etc... (see np.linalg)
- Make broadcasting work i.e. make a matrix from two vectors:
```python
import numpy as np

vec1 = np.random.rand(10, 1)
vec2 = np.random.rand(1, 12)

matrix = vec1 * vec2  # Broadcasting
print(matrix.shape)  # (10, 12)

```
- Add the `nx::where()` function.
