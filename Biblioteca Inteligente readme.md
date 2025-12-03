#  Biblioteca Inteligente

##  Descripción del Proyecto

Este proyecto implementa un sistema de gestión de biblioteca completo en C++. Va más allá de la simple gestión de inventario, se incorporaron estructuras de datos complejas para ofrecer funcionalidades avanzadas como búsqueda eficiente y un sistema de recomendación basado en el historial de préstamos.

La persistencia de los datos se realiza mediante archivos CSV.

##  Estructuras de Datos y Algoritmos Implementados

| Estructura/Algoritmo | Propósito Principal | Implementación en el Código |
| :--- | :--- | :--- |
| **AVL (Árbol Binario de Búsqueda Auto-Balanceado)** | Almacena libros, ordenados por su **ISBN numérico**, permitiendo un acceso y listado en orden rápido (O(log *n*)). | `AVL` struct, utilizado por `isbn_avl`. |
| **Trie (Árbol de Prefijos)** | Optimiza la **función de autocompletado** para la búsqueda de títulos y autores. | `Trie` struct. |
| **Grafo No Dirigido (Mapa de Adyacencia)** | Se construye un grafo donde los nodos son libros. Un peso en la arista ($L_1 \leftrightarrow L_2$) indica cuántos usuarios han leído juntos los libros $L_1$ y $L_2$. | `grafico_libro` (`unordered_map<string, unordered_map<string, int>>`). |
| **Algoritmo de Recomendación** | Implementa un **filtrado colaborativo** simple basado en el grafo de libros, sugiriendo ítems leídos por usuarios con gustos similares. | Función `recomendar_para_usuario()`. |
| **Mapas Hash (`unordered_map`)** | Utilizados para el acceso rápido (O(1) promedio) a libros por ISBN y a usuarios por ID. | `libros`, `usuarios`, `mapa_busqueda`. |
| **Cola (`queue`)** | Gestiona la **lista de espera** para los libros sin copias disponibles. | `lista_espera` (`unordered_map<string, queue<string>>`). |
| **Pila (implícita en `vector`)** | El historial de acciones (`historial_acciones`) funciona como una pila para implementar la función **Deshacer la última operación**. | `vector<Accion> historial_acciones`. |

##  Cómo Compilar y Ejecutar

El proyecto fue desarrollado en C++.

1. **Guardar el Código:** Asegúrate de que el código fuente esté guardado como `Biblioteca.cpp`.
2. **Compilación (ejemplo con g++):**
    ```bash
    g++ -std=c++11 Biblioteca.cpp -o biblioteca_app
    ```
3. **Ejecución:**
    ```bash
    ./biblioteca_app
    ```

###  Persistencia de Datos

Al inicio, la aplicación intentará cargar los datos de los siguientes archivos CSV. Si no existen, los creará vacíos al salir o al realizar la primera operación que los requiera:

* `libros.csv`
* `usuarios.csv`
* `prestamos.csv`
* `lista_espera.csv`

##  Funcionalidades Principales y Menú 

| Opción | Categoría | Descripción | Estructura Involucrada |
| :--- | :--- | :--- | :--- |
| **1.** | **Libro** | **Agregar libro** | Registra un nuevo libro. Genera un ISBN si es necesario y lo inserta en el **AVL** y el **Trie**. |
| **2.** | **Libro** | **Eliminar libro (por ISBN)** | Elimina el registro de un libro de todas las estructuras de datos. |
| **3.** | **Usuario** | **Agregar usuario** | Crea un nuevo usuario, asignándole un ID aleatorio único. |
| **4.** | **Usuario** | **Eliminar usuario (por ID)** | Elimina el registro de un usuario del sistema. |
| **5.** | **Usuario** | **Mostrar todos los usuarios** | Lista todos los usuarios registrados en el sistema. |
| **6.** | **Préstamo** | **Prestar libro** | Gestiona un préstamo. Si no hay copias, pregunta si desea colocar al usuario en la **Cola (`queue`)** de espera. |
| **7.** | **Préstamo** | **Devolver libro** | Procesa la devolución. Si hay usuarios en lista de espera, asigna el libro al siguiente en la cola. |
| **8.** | **Búsqueda** | **Buscar título (Autocompletar)** | Búsqueda inteligente de títulos y autores basada en prefijos, utilizando el **Trie**. |
| **9.** | **Búsqueda** | **Mostrar todos los libros** | Muestra el inventario completo de libros (sin orden específico). |
| **10.** | **Recomendación** | **Recomendar libros (por Usuario)** | Sugiere libros basándose en el historial de préstamos de otros usuarios, utilizando el **Grafo de Adyacencia**. |
| **11.** | **Detalle** | **Mostrar libro ** | Muestra toda la información de un libro específico. |
| **12.** | **Detalle** | **Mostrar usuario ** | Muestra toda la información de un usuario específico, incluyendo su historial de préstamos. |
| **13.** | **Listado** | **Listar libros por ISBN Numérico** | Muestra el inventario **ordenado** ascendentemente por el valor numérico del ISBN, demostrando el recorrido in-orden del **AVL**. |
| **14.** | **Control** | **Deshacer la última acción** | Revierte la última operación de modificación de datos realizada (solo para algunas operaciones de adición/préstamo). |
| **15.** | **Búsqueda** | **Mostrar libros por Género** | Muestra todos los libros que pertenecen al género especificado. |
| **16.** | **Control** | **Salir** | Guarda todos los datos en los archivos CSV y termina la aplicación. |