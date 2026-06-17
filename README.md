# Blockchain de Votación Segura

Simulación de un sistema de votación electrónica distribuida usando una blockchain simple y el patrón de diseño **Observer**.

## ¿Cómo funciona?

- Los votos se agrupan en **bloques** encadenados mediante hashes. Si alguien altera un voto pasado, el hash del bloque cambia y toda la cadena queda inválida.
- Cada bloque debe ser **minado**: se busca por fuerza bruta un número (nonce) tal que el hash resultante empiece con una cantidad de ceros según la dificultad configurada.
- No hay servidor central. Hay **3 mesas electorales** independientes, cada una con su propia copia de la cadena.
- Cuando una mesa termina de minar un bloque, notifica automáticamente a las demás mediante el patrón **Observer**, y estas verifican y agregan el bloque a su cadena local.

## Estructura del proyecto

```
main.cpp   — código fuente completo
README.md  — este archivo
```

## Clases principales

| Clase | Rol |
|---|---|
| `Voto` | Guarda el ID del votante y la opción elegida |
| `Block` | Contiene los votos, el hash anterior, el nonce y su propio hash |
| `Blockchain` | Administra la cadena de bloques y valida su integridad |
| `MesaElectoralObserver` | Interfaz del patrón Observer |
| `MesaElectoral` | Nodo de la red; mina bloques y recibe actualizaciones |
| `CentroElectoralSubject` | Notifica a todas las mesas cuando se mina un bloque nuevo |

## Mejoras aplicadas

### Mejora A — Parámetros por `const&`
Los parámetros de tipo pesado (`string`, `vector<Voto>`) se pasan ahora por referencia constante en lugar de por valor. Esto evita copias innecesarias en cada llamada a funciones como `generarHash()`, el constructor de `Block`, `registrarVotos()`, y otros.

```cpp
// Antes
string generarHash(string texto)
Block registrarVotos(vector<Voto> votos)

// Después
string generarHash(const string& texto)
Block registrarVotos(const vector<Voto>& votos)
```

### Mejora B — Encapsulamiento de `Block`
Los atributos de la clase `Block` eran públicos, lo que permitía modificarlos directamente desde cualquier parte del código. Ahora son **privados** y se accede a ellos únicamente mediante getters.

```cpp
// Antes
bloque.index
bloque.currentHash

// Después
bloque.getIndex()
bloque.getCurrentHash()
```

## Compilación y ejecución

```bash
g++ -o votacion main.cpp
./votacion
```

## Ejemplo de salida

```
Mesa Electoral 1 esta minando un nuevo bloque...
Bloque #1 fue minado correctamente.
Nonce encontrado: 250
Hash final: 00e6fd4535e588d9

Mesa Electoral 2 recibio un bloque nuevo.
Mesa Electoral 2 verifico el bloque y lo agrego a su cadena.

Resultado de la validacion final:
Mesa 1: cadena valida
Mesa 2: cadena valida
Mesa 3: cadena valida
```
