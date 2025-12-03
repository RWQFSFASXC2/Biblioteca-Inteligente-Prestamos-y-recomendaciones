// PROYECTO FINAL - ESTRUCTURA DE DATOS
// Inclusión de la librería estándar de entrada/salida
#include <iostream>
// Inclusión de la librería para usar vectores dinámicos
#include <vector>
// Inclusión de la librería para manejar cadenas de texto
#include <string>
// Inclusión de mapas desordenados (Hash Maps)
#include <unordered_map>
// Inclusión de conjuntos desordenados (Hash Sets)
#include <unordered_set>
// Inclusión de mapas ordenados (Árboles Rojo-Negro generalmente)
#include <map>
// Inclusión de algoritmos estándar (sort, transform, etc.)
#include <algorithm>
// Inclusión de colas (FIFO)
#include <queue>
// Inclusión de streams de cadenas para manipulación de strings
#include <sstream>
// Inclusión de streams de archivos para leer/escribir ficheros
#include <fstream>
// Inclusión de funciones para clasificación de caracteres (isdigit, tolower)
#include <cctype>
// Inclusión de herramientas para generación de números aleatorios
#include <random>

// Uso del espacio de nombres estándar para evitar escribir std::
using namespace std;

// Función para generar un ID aleatorio numérico de longitud fija
string generar_id_aleatorio(size_t len = 12) {
    // Declaración estática del dispositivo de entropía (se inicializa una vez)
    static random_device rd;
    // Declaración estática del motor de generación aleatoria
    static default_random_engine generator(rd());
    
    // Definición de los caracteres permitidos para el ID
    const string caracteres = "0123456789";
    // Configuración de la distribución uniforme basada en la longitud de caracteres
    uniform_int_distribution<int> distribution(0, (int)caracteres.length() - 1);
    // Variable para almacenar el ID resultante
    string random_id;
    // Bucle para generar cada carácter del ID
    for (size_t i = 0; i < len; ++i) {
        // Añade un carácter aleatorio seleccionado de la cadena 'caracteres'
        random_id += caracteres[distribution(generator)];
    }
    // Retorna el ID generado
    return random_id;
}

// Función para eliminar espacios en blanco al inicio y final de un string
string trim(const string& str) {
    // Definición de los caracteres considerados espacios en blanco
    const string whitespace = " \t\n\r";
    // Busca la posición del primer carácter que NO sea espacio
    size_t first = str.find_first_not_of(whitespace);
    // Si no se encuentra ningún carácter (cadena vacía o solo espacios)
    if (string::npos == first) {
        // Retorna una cadena vacía
        return "";
    }
    // Busca la posición del último carácter que NO sea espacio
    size_t last = str.find_last_not_of(whitespace);
    // Retorna la subcadena desde el primer carácter válido hasta el último
    return str.substr(first, (last - first + 1));
}

// ------------------ Trie -----------------
// Estructura de datos Trie (Árbol de prefijos) para búsquedas de texto
struct Trie {
    // Definición del nodo del Trie
    struct Nodo {
        // Mapa para guardar los hijos del nodo (carácter -> índice en vector)
        unordered_map<char, int> siguiente;
        // Bandera para indicar si este nodo marca el final de una palabra
        bool fin = false;
    };
    // Vector que almacena todos los nodos del Trie (implementación dinámica)
    // Se inicializa con un nodo raíz vacío
    vector<Nodo> tabla = { Nodo() };

    // Función para insertar una palabra en el Trie
    void insertar(const string& s) {
        // Índice del nodo actual, comienza en la raíz (0)
        int u = 0;
        // Recorre cada carácter de la cadena
        for (char c : s) {
            // Si el carácter no existe en los hijos del nodo actual
            if (!tabla[u].siguiente.count(c)) {
                // Asigna el índice del nuevo nodo (tamaño actual del vector)
                tabla[u].siguiente[c] = (int)tabla.size();
                // Añade un nuevo nodo vacío al vector
                tabla.push_back(Nodo());
            }
            // Avanza al siguiente nodo
            u = tabla[u].siguiente[c];
        }
        // Marca el nodo final como fin de palabra
        tabla[u].fin = true;
    }

    // --- Función auxiliar recursiva para recolectar palabras ---
    // Recorre el trie desde un nodo dado para encontrar todas las palabras completas
    void recolectar(int u, string prefijo_actual, vector<string>& resultados) const {
        // Si el nodo actual marca el fin de una palabra
        if (tabla[u].fin) {
            // Añade la palabra formada hasta ahora a los resultados
            resultados.push_back(prefijo_actual);
        }
        // Itera sobre todos los hijos del nodo actual
        for (auto const& [key, next_node_index] : tabla[u].siguiente) {
            // Llamada recursiva avanzando al siguiente nodo y añadiendo el carácter al prefijo
            recolectar(next_node_index, prefijo_actual + key, resultados);
        }
    }

    // --- Función principal de búsqueda ---
    // Busca todas las palabras que comienzan con un prefijo dado
    vector<string> buscarPrefijo(const string& prefijo) const {
        // Vector para almacenar los resultados
        vector<string> resultados;
        // Índice del nodo actual, comienza en raíz
        int u = 0;
        // 1. Navegar hasta el final del prefijo proporcionado
        for (char c : prefijo) {
            // Si en algún punto el camino no existe
            if (!tabla[u].siguiente.count(c)) {
                // Retorna la lista vacía porque el prefijo no está en el Trie
                return resultados; 
            }
            // Avanza al siguiente nodo según el carácter
            u = tabla[u].siguiente.at(c);
        }
        // 2. Desde el nodo donde termina el prefijo, recolectar todas las terminaciones posibles
        recolectar(u, prefijo, resultados);
        // Retorna las palabras encontradas
        return resultados;
    }
};

// ------------------ AVL -----------------
// Estructura de árbol AVL para mantener ISBNs ordenados y balanceados
struct AVL {
    // Definición del nodo del árbol AVL
    struct N {
        // Clave numérica para el ordenamiento (ISBN numérico)
        long long clave;
        // Cadena original del ISBN (para preservar ceros iniciales o formato)
        string isbn_str;
        // Altura del nodo para el balanceo
        int altura;
        // Puntero al hijo izquierdo
        N* izquierda;
        // Puntero al hijo derecho
        N* derecha;
        // Constructor del nodo
        N(long long k, const string& s) : clave(k), isbn_str(s), altura(1), izquierda(nullptr), derecha(nullptr) {}
    };
    // Puntero a la raíz del árbol, inicializado en null
    N* raiz = nullptr;

    // Función estática para recorrido In-Order (Izquierda - Raíz - Derecha)
    static void recorridoInOrder(N* n, vector<pair<long long, string>>& claves) {
        // Caso base: si el nodo es nulo, retornar
        if (!n) {
            return;
        }
        // Llamada recursiva al hijo izquierdo
        recorridoInOrder(n->izquierda, claves);
        // Procesar el nodo actual (añadir al vector)
        claves.push_back({ n->clave, n->isbn_str });
        // Llamada recursiva al hijo derecho
        recorridoInOrder(n->derecha, claves);
    }
    
    // Función pública para obtener claves ordenadas
    vector<pair<long long, string>> obtenerClavesOrdenadas() const {
        // Vector para almacenar los pares (clave, isbn)
        vector<pair<long long, string>> claves;
        // Iniciar el recorrido desde la raíz
        recorridoInOrder(raiz, claves);
        // Retornar el vector lleno
        return claves;
    }

    // Función auxiliar para obtener la altura de un nodo de forma segura
    static int Altura(N* n) {
        // Retorna la altura si el nodo existe, si no, retorna 0
        return n ? n->altura : 0;
    }
    
    // Función para actualizar la altura de un nodo basado en sus hijos
    static void actualizar(N* n) {
        // Si el nodo existe
        if (n) {
            // La altura es 1 + el máximo de la altura de sus hijos
            n->altura = 1 + max(Altura(n->izquierda), Altura(n->derecha));
        }
    }
    
    // Función para calcular el factor de balance (Izquierda - Derecha)
    static int factorBalance(N* n) {
        // Retorna la diferencia de alturas si n existe, sino 0
        return n ? Altura(n->izquierda) - Altura(n->derecha) : 0;
    }

    // Rotación simple a la derecha para balancear
    static N* rotacionDerecha(N* y) {
        // Guardar el hijo izquierdo de y en x
        N* x = y->izquierda;
        // Guardar el hijo derecho de x en T
        N* T = x->derecha;
        // Realizar la rotación: y se convierte en hijo derecho de x
        x->derecha = y;
        // T se convierte en hijo izquierdo de y
        y->izquierda = T;
        // Actualizar alturas de los nodos afectados (primero y, luego x)
        actualizar(y);
        actualizar(x);
        // Retornar x como nueva raíz del subárbol
        return x;
    }
    
    // Rotación simple a la izquierda para balancear
    static N* rotacionIzquierda(N* x) {
        // Guardar el hijo derecho de x en y
        N* y = x->derecha;
        // Guardar el hijo izquierdo de y en T
        N* T = y->izquierda;
        // Realizar la rotación: x se convierte en hijo izquierdo de y
        y->izquierda = x;
        // T se convierte en hijo derecho de x
        x->derecha = T;
        // Actualizar alturas de los nodos afectados
        actualizar(x);
        actualizar(y);
        // Retornar y como nueva raíz del subárbol
        return y;
    }
    
    // Función recursiva para insertar un nodo en el AVL
    static N* insercion(N* n, long long k, const string& s) {
        // Caso base: si llegamos a un puntero nulo, creamos el nuevo nodo
        if (!n) {
            return new N(k, s);
        }
        // Si la clave a insertar es menor, ir a la izquierda
        if (k < n->clave) {
            n->izquierda = insercion(n->izquierda, k, s);
        }
        // Si la clave a insertar es mayor, ir a la derecha
        else if (k > n->clave) {
            n->derecha = insercion(n->derecha, k, s);
        }
        // Si la clave es igual, no hacemos nada (no permitimos duplicados de clave en este AVL)
        else {
            return n;
        }
        
        // Actualizar la altura del nodo actual tras la inserción
        actualizar(n);
        // Calcular el factor de balance
        int b = factorBalance(n);
        
        // Caso: Desbalanceo Izquierda-Izquierda
        if (b > 1 && k < n->izquierda->clave) {
            return rotacionDerecha(n);
        }
        // Caso: Desbalanceo Derecha-Derecha
        if (b < -1 && k > n->derecha->clave) {
            return rotacionIzquierda(n);
        }
        // Caso: Desbalanceo Izquierda-Derecha
        if (b > 1 && k > n->izquierda->clave) {
            // Convertir a Izquierda-Izquierda y luego rotar
            n->izquierda = rotacionIzquierda(n->izquierda);
            return rotacionDerecha(n);
        }
        // Caso: Desbalanceo Derecha-Izquierda
        if (b < -1 && k < n->derecha->clave) {
            // Convertir a Derecha-Derecha y luego rotar
            n->derecha = rotacionDerecha(n->derecha);
            return rotacionIzquierda(n);
        }
        // Retornar el nodo (posiblemente la nueva raíz del subárbol)
        return n;
    }
    
    // Función pública para insertar en el árbol
    void insertar(long long k, const string& s) { 
        // Llama a la función recursiva y actualiza la raíz
        raiz = insercion(raiz, k, s); 
    }
};
// ------------------ Modelos de datos -----------------

// Estructura que representa un libro en la biblioteca
struct Libro {
    // Identificador único del libro (cadena de texto)
    string isbn;
    // Versión numérica del ISBN para ordenamiento eficiente en AVL
    long long isbn_num = 0;
    // Título del libro
    string titulo;
    // Lista de autores del libro
    vector<string> autores;
    // Género literario
    string genero;
    // Fecha de publicación en formato YYYY-MM-DD
    string fecha_publi;
    // Cantidad total de copias físicas que posee la biblioteca
    int copias_totales = 0;
    // Cantidad de copias que están actualmente en estante (no prestadas)
    int copias_disponibles = 0;
};

// Estructura que representa un usuario de la biblioteca
struct Usuario {
    // Identificador único del usuario
    string id_usuario;
    // Nombre completo del usuario
    string nombre;
    // Correo electrónico de contacto
    string correo;
    // Historial de ISBNs de libros que el usuario ha tomado prestados (histórico completo)
    vector<string> historial_isbn;
    // Conjunto de ISBNs que el usuario tiene prestados ACTUALMENTE (búsqueda rápida)
    unordered_set<string> prestamos_activos;
    // Contador de cuántos libros tiene prestados en este momento
    int num_prestamos_activos = 0;
    // Historial de títulos de libros leídos (útil si el libro se borra del sistema después)
    vector<string> historial_titulos;
};

// Estructura que representa un préstamo activo o inactivo
struct Prestamo {
    // Identificador único de la transacción de préstamo
    string id_prestamo;
    // Título del libro prestado (snapshot al momento del préstamo)
    string titulo;
    // ISBN del libro prestado
    string isbn;
    // ID del usuario que tiene el libro
    string id_usuario;
    // Estado del préstamo: true si el libro aún no se ha devuelto, false si ya se devolvió
    bool activo = true;
};

// ------------------ Biblioteca -----------------
// Clase principal que gestiona toda la lógica del sistema
class Biblioteca {
private:
    // --- Persistencia CSV ---
    // Nombre del archivo para guardar libros
    const string LIBROS_CSV = "libros.csv";
    // Nombre del archivo para guardar usuarios
    const string USUARIOS_CSV = "usuarios.csv";
    // Nombre del archivo para guardar préstamos
    const string PRESTAMOS_CSV = "prestamos.csv";
    // Nombre del archivo para guardar la lista de espera
    const string LISTA_ESPERA_CSV = "lista_espera.csv";
    // Delimitador usado en los archivos CSV (coma)
    static const char DELIMITADOR = ',';

    // Base de datos en memoria: ISBN -> Objeto Libro
    unordered_map<string, Libro> libros;
    // Base de datos en memoria: ID Usuario -> Objeto Usuario
    unordered_map<string, Usuario> usuarios;
    // Cola de espera: ISBN -> Cola de IDs de usuarios esperando ese libro
    unordered_map<string, queue<string>> lista_espera;
    // Índice secundario ordenado: Título -> ISBN (permite listar alfabéticamente)
    map<string, string> indice;
    // Estructura Trie para autocompletado y búsqueda por prefijo
    Trie trie;
    // Árbol AVL para ordenar libros por ISBN numérico
    AVL isbn_avl;
    // Mapa para conectar: Texto de búsqueda (normalizado) -> Lista de ISBNs coincidentes
    unordered_map<string, vector<string>> mapa_busqueda;
    // Base de datos en memoria de préstamos: ID Préstamo -> Objeto Préstamo
    unordered_map<string, Prestamo> prestamos;
    // Grafo para recomendaciones: ISBN A -> (ISBN B -> Peso de conexión)
    unordered_map<string, unordered_map<string, int>> grafico_libro;
    // Mapa auxiliar (parece redundante con Usuario::historial_isbn, pero se mantiene según original)
    unordered_map<string, unordered_set<string>> libros_usuario;

    // Enumeración para definir los tipos de acciones que se pueden deshacer
   enum class TipoAccion { 
        AgregarLibro,
        EliminarLibro,
        ModificarLibro,
        AgregarUsuario,
        EliminarUsuario,
        PrestarLibro,
        DevolverLibro,
        PonerenCola
    };
    
    // Estructura para registrar una acción en el historial (Pila de deshacer)
    struct Accion {
        TipoAccion tipo;
        string id;      // ID principal afectado (ej. ISBN o ID Usuario)
        string usuario; // ID secundario afectado (ej. Usuario en préstamo)
        string datos;   // Datos extra si se requieren para restaurar estado
    };

    // Vector que funciona como pila para almacenar el historial de acciones
    vector<Accion> historial_acciones;

    // Función privada para añadir una acción al historial
    void registrar_accion(const Accion& a) { 
        // Empuja la acción al final del vector
        historial_acciones.push_back(a); 
    }
    
    // --- Manejo de comillas/comas en CSV ---
    // Escapa cadenas para formato CSV (si tienen comas o comillas, las envuelve en comillas)
    static string csv_quote(const string& s) {
        // Verifica si la cadena necesita ser escapada (contiene delimitador o comillas)
        if (s.find(DELIMITADOR) == string::npos && s.find('"') == string::npos) {
            // Si es segura, la retorna tal cual
            return s;
        }
        // Inicia la cadena de salida con una comilla de apertura
        string out = "\"";
        // Recorre cada carácter de la cadena original
        for (char c : s) {
            // Si encuentra una comilla doble, debe duplicarla para escaparla (regla CSV)
            if (c == '"') {
                out += "\"\"";
            }
            else {
                // Si es un carácter normal, lo añade
                out.push_back(c);
            }
        }
        // Cierra la cadena con una comilla
        out += "\"";
        // Retorna la cadena formateada
        return out;
    }

    // Parser CSV: Divide una línea en campos respetando comillas
    vector<string> parse_csv_line(const string& line, char delimiter) {
        // Vector para guardar los campos resultantes
        vector<string> result;
        // String temporal para construir el campo actual
        string field;
        // Bandera para saber si estamos dentro de un bloque entrecomillado
        bool in_quotes = false;
        
        // Itera sobre cada carácter de la línea
        for (size_t i = 0; i < line.length(); ++i) {
            char c = line[i];
            
            // Si encontramos una comilla
            if (c == '"') {
                // Verificar si es una comilla escapada (doble comilla "")
                if (in_quotes && i + 1 < line.length() && line[i + 1] == '"') {
                    // Añadir una sola comilla al campo
                    field += '"';
                    // Saltar el siguiente carácter porque ya lo procesamos
                    i++;
                }
                else {
                    // Si no es escapada, invertimos el estado (entrar/salir de comillas)
                    in_quotes = !in_quotes;
                }
            }
            // Si encontramos el delimitador Y NO estamos dentro de comillas
            else if (c == delimiter && !in_quotes) {
                // El campo ha terminado, lo guardamos en el vector
                result.push_back(field);
                // Limpiamos el string temporal para el siguiente campo
                field.clear();
            }
            // Carácter normal
            else {
                // Lo añadimos al campo actual
                field += c;
            }
        }
        // Añadimos el último campo (lo que quedó en 'field' tras el bucle)
        result.push_back(field);
        // Retornamos el vector de campos
        return result;
    }

    // Función auxiliar para unir un vector de strings en una sola cadena con separador
    static string join(const vector<string>& v, const string& sep) {
        // String acumulador
        string out;
        // Itera por los elementos del vector
        for (size_t i = 0; i < v.size(); ++i) {
            // Si no es el primer elemento, añade el separador antes
            if (i) {
                out += sep;
            }
            // Añade el elemento actual
            out += v[i];
        }
        // Retorna la cadena unida
        return out;
    }
    // --- Funciones de CSV ---
    
    // Función para cargar los libros desde el archivo CSV al iniciar el programa
    void cargarLibrosCSV() {
        // Abre el archivo de lectura
        ifstream file(LIBROS_CSV);
        // Verifica si el archivo se abrió correctamente
        if (!file.is_open()) {
            // Si falla, imprime mensaje de error
            cout << "Archivo " << LIBROS_CSV << " no encontrado." << endl;
            // Sale de la función
            return;
        }
        // Variable para almacenar cada línea leída
        string line;
        // Lee la primera línea (encabezado) para descartarla
        getline(file, line); 
        
        // Bucle para leer el resto del archivo línea por línea
        while (getline(file, line)) {
            // Parsea la línea separando por comas y respetando comillas
            vector<string> campos = parse_csv_line(line, DELIMITADOR);
            
            // Verifica que la línea tenga al menos los 7 campos requeridos
            if (campos.size() >= 7) {
                // Crea un objeto Libro temporal
                Libro l;
                // Asigna el ISBN limpiando espacios (campo 0)
                l.isbn = trim(campos[0]);
                
                // Limpia el ISBN dejando solo dígitos para consistencia
                l.isbn.erase(remove_if(l.isbn.begin(), l.isbn.end(),
                    [](unsigned char c) { return !isdigit(c); }), l.isbn.end());
                
                // Asigna el título limpiando espacios (campo 1)
                l.titulo = trim(campos[1]);
                
                // Procesa los autores (campo 2), separados internamente por '^'
                stringstream ss_autores(campos[2]);
                string autor;
                // Limpia el vector de autores por seguridad
                l.autores.clear();
                
                // Itera sobre el stringstream usando '^' como delimitador
                while (getline(ss_autores, autor, '^')) {
                    // Si el autor extraído no está vacío
                    if (!autor.empty()) {
                        // Lo añade a la lista de autores del libro
                        l.autores.push_back(autor);
                    }
                }
                
                // Asigna el género (campo 3)
                l.genero = trim(campos[3]);
                // Asigna la fecha de publicación (campo 4)
                l.fecha_publi = campos[4];
                
                // Bloque try-catch para convertir strings a enteros (copias)
                try {
                    // Convierte copias totales (campo 5)
                    l.copias_totales = stoi(campos[5]);
                    // Convierte copias disponibles (campo 6)
                    l.copias_disponibles = stoi(campos[6]);
                }
                catch (const invalid_argument&) {
                    // Si falla la conversión, muestra error
                    cerr << "Error al convertir copias en linea: " << line << endl;
                    // Salta a la siguiente iteración del while (ignora este libro)
                    continue;
                }
                
                // Bloque try-catch para convertir ISBN a número (para AVL)
                try {
                    // Intenta convertir el string ISBN a long long
                    l.isbn_num = stoll(l.isbn);
                }
                catch (...) {
                    // Si falla (por ejemplo contiene letras), asigna 0
                    l.isbn_num = 0;
                }
                
                // Guarda el libro en el mapa principal usando el ISBN como clave
                libros[l.isbn] = l;
                // Actualiza el índice Título -> ISBN
                indice[l.titulo] = l.isbn;
                
                // Llama a función auxiliar para indexar el Título en el Trie
                indexar_termino(l.titulo, l.isbn); 
                
                // Itera sobre los autores para indexarlos también
                for (const string& autor : l.autores) {
                    // Indexa cada autor en el Trie vinculado a este ISBN
                    indexar_termino(autor, l.isbn); 
                }
                
                // Si el ISBN numérico es válido, lo inserta en el árbol AVL
                if (l.isbn_num != 0) {
                    isbn_avl.insertar(l.isbn_num, l.isbn);
                }
            }
        }
        // Cierra el archivo
        file.close();
        // Mensaje de confirmación
        cout << "Libros cargados desde " << LIBROS_CSV << endl;
    }

    // Función para guardar el estado actual de los libros en el CSV
    void guardarLibrosCSV() {
        // Abre el archivo para escritura (sobreescribe contenido previo)
        ofstream file(LIBROS_CSV);
        // Verifica si se pudo abrir
        if (!file.is_open()) {
            // Muestra error si falla
            cerr << "Error, no se pudo abrir " << LIBROS_CSV << endl;
            return;
        }
        // Escribe el encabezado del CSV
        file << "isbn,titulo,autores,genero,fecha_publi,copias_totales,copias_disponibles\n";
        
        // Itera sobre todos los libros en memoria
        for (const auto& pair : libros) {
            // Obtiene referencia al objeto libro
            const Libro& b = pair.second;
            // Convierte el vector de autores a string separado por '^'
            string autores_str = join(b.autores, "^");
            
            // Escribe los campos en el archivo, usando csv_quote para seguridad
            file << csv_quote(b.isbn) << DELIMITADOR
                << csv_quote(b.titulo) << DELIMITADOR
                << csv_quote(autores_str) << DELIMITADOR
                << csv_quote(b.genero) << DELIMITADOR
                << csv_quote(b.fecha_publi) << DELIMITADOR
                << b.copias_totales << DELIMITADOR
                << b.copias_disponibles << "\n"; // Salto de línea al final
        }
        // Cierra el archivo
        file.close();
    }

    // Función para construir el grafo de recomendaciones basado en historiales
    void inicializarGrafo() {
        // Limpia cualquier dato previo en el grafo
        grafico_libro.clear();
        
        // Itera sobre todos los usuarios registrados
        for (const auto& par_u : usuarios) {
            // Obtiene referencia al usuario
            const Usuario& u = par_u.second;
            
            // Bucle anidado para comparar todos los libros que ha leído este usuario entre sí
            // Itera desde el primer libro del historial
            for (size_t i = 0; i < u.historial_isbn.size(); ++i) {
                // Itera desde el siguiente libro (para hacer pares únicos)
                for (size_t j = i + 1; j < u.historial_isbn.size(); ++j) {
                    // Obtiene ISBN del primer libro
                    const string& isbn1 = u.historial_isbn[i];
                    // Obtiene ISBN del segundo libro
                    const string& isbn2 = u.historial_isbn[j];
                    
                    // Verifica que ambos libros sigan existiendo en la base de datos
                    if (libros.count(isbn1) && libros.count(isbn2)) {
                        // Incrementa el peso de la conexión isbn1 -> isbn2
                        grafico_libro[isbn1][isbn2] += 1;
                        // Incrementa el peso de la conexión inversa (grafo no dirigido)
                        grafico_libro[isbn2][isbn1] += 1;
                    }
                }
            }
        }
    }
    // Cargar Usuarios desde el archivo CSV
    void cargarUsuariosCSV() {
        // Abre el archivo de usuarios para lectura
        ifstream file(USUARIOS_CSV);
        
        // Verifica si el archivo no se pudo abrir (probablemente no existe)
        if (!file.is_open()) {
            // Informa al usuario que el archivo no existe
            cout << "Archivo " << USUARIOS_CSV << " no encontrado. Creando archivo vacío." << endl;
            // Crea un archivo vacío para evitar errores futuros
            guardarUsuariosCSV();
            // Sale de la función
            return;
        }
        
        // Variable para leer líneas
        string line;
        // Lee la primera línea (cabecera) y la descarta
        getline(file, line); 
        
        // Bucle para leer cada línea de datos del usuario
        while (getline(file, line)) {
            // Parsea la línea usando la coma como delimitador
            vector<string> campos = parse_csv_line(line, DELIMITADOR);
            
            // Verifica que la línea tenga los 6 campos esperados
            // id, nombre, correo, prestamos_activos, historial_isbn, historial_titulos
            if (campos.size() >= 6) {
                // Crea un objeto Usuario temporal
                Usuario u;
                // Asigna ID (campo 0)
                u.id_usuario = campos[0];
                // Asigna Nombre (campo 1)
                u.nombre = campos[1];
                // Asigna Correo (campo 2)
                u.correo = campos[2];
                
                // --- Deserializar prestamos_activos (Campo 3) ---
                // Limpia el conjunto por seguridad
                u.prestamos_activos.clear();
                // Crea un stream con el contenido del campo 3
                stringstream ss_pa(campos[3]);
                string isbn_pa;
                // Itera sobre el stream usando '^' como separador de ISBNs
                while (getline(ss_pa, isbn_pa, '^')) {
                    // Si el segmento no está vacío
                    if (!isbn_pa.empty()) {
                        // Lo inserta en el conjunto de préstamos activos
                        u.prestamos_activos.insert(isbn_pa);
                    }
                }
                
                // --- Deserializar historial_isbn (Campo 4) ---
                // Limpia el vector por seguridad
                u.historial_isbn.clear();
                // Crea un stream con el contenido del campo 4
                stringstream ss_h(campos[4]);
                string isbn_h;
                // Itera sobre el stream usando '^' como separador
                while (getline(ss_h, isbn_h, '^')) {
                    // Si el segmento no está vacío
                    if (!isbn_h.empty()) {
                        // Lo añade al historial de ISBNs
                        u.historial_isbn.push_back(isbn_h);
                    }
                }
                
                // --- Deserializar historial_titulos (Campo 5) ---
                // Limpia el vector por seguridad
                u.historial_titulos.clear();
                // Crea un stream con el contenido del campo 5
                stringstream ss_t(campos[5]);
                string tit;
                // Itera sobre el stream usando '^' como separador
                while (getline(ss_t, tit, '^')) {
                    // Si el título no está vacío
                    if (!tit.empty()) {
                        // Lo añade al historial de títulos
                        u.historial_titulos.push_back(tit);
                    }
                }
                
                // Guarda el usuario reconstruido en el mapa principal
                usuarios[u.id_usuario] = u;
            }
        }
        // Cierra el archivo
        file.close();
        // Mensaje de éxito
        cout << "Usuarios cargados desde " << USUARIOS_CSV << endl;
    }

    // Guardar Usuarios en el archivo CSV
    void guardarUsuariosCSV() {
        // Abre el archivo para escritura (trunca el contenido existente)
        ofstream file(USUARIOS_CSV);
        
        // Verifica si el archivo se abrió correctamente
        if (!file.is_open()) {
            // Muestra error si no se pudo abrir
            cerr << "Error: No se pudo abrir " << USUARIOS_CSV << " para escritura." << endl;
            return;
        }
        
        // Escribe la cabecera del CSV
        file << "id_usuario,nombre,correo,prestamos_activos,historial_isbn,historial_titulos_lectura\n";
        
        // Itera sobre todos los usuarios en memoria
        for (const auto& pair : usuarios) {
            // Obtiene referencia al usuario actual
            const Usuario& u = pair.second;
            
            // Serializa el set de préstamos activos a un string separado por '^'
            string prestamos_str = join(vector<string>(u.prestamos_activos.begin(), u.prestamos_activos.end()), "^");
            
            // Serializa el historial de ISBNs a un string separado por '^'
            string historial_isbn_str = join(u.historial_isbn, "^");
            
            // --- Lógica para reconstruir historial de títulos ---
            // Vector temporal para los títulos
            vector<string> historial_titulos_vec;
            
            // Recorre el historial de ISBNs para buscar los títulos actuales
            for (const string& isbn : u.historial_isbn) {
                // Si el libro existe en la base de datos actual
                if (libros.count(isbn)) {
                    // Obtiene el título real y actualizado
                    historial_titulos_vec.push_back(libros.at(isbn).titulo);
                }
                else {
                    // Si el libro fue borrado, marca como NO ENCONTRADO temporalmente
                    historial_titulos_vec.push_back("ISBN_NO_ENCONTRADO");
                }
            }
            
            // Si el usuario ya tenía un historial de títulos explícito (cargado o guardado previamente)
            // Esto es crucial para preservar títulos de libros que ya se eliminaron de la biblioteca
            if (!u.historial_titulos.empty()) {
                // Sobreescribe la versión reconstruida con la versión histórica
                historial_titulos_vec = u.historial_titulos;
            }
            
            // Serializa el vector de títulos a string separado por '^'
            string historial_titulos_str = join(historial_titulos_vec, "^");
            
            // Escribe la línea completa en el CSV, entrecomillando cada campo
            file << csv_quote(u.id_usuario) << DELIMITADOR
                << csv_quote(u.nombre) << DELIMITADOR
                << csv_quote(u.correo) << DELIMITADOR
                << csv_quote(prestamos_str) << DELIMITADOR
                << csv_quote(historial_isbn_str) << DELIMITADOR
                << csv_quote(historial_titulos_str) << "\n";
        }
        // Cierra el archivo
        file.close();
    }
    // --- Cargar/Guardar prestamos ---
    
    // Carga los préstamos desde el archivo CSV
    void cargarPrestamosCSV() {
        // Abre el archivo de préstamos en modo lectura
        ifstream file(PRESTAMOS_CSV);
        
        // Verifica si el archivo no existe o no se pudo abrir
        if (!file.is_open()) {
            // Informa que no se encontró el archivo
            cout << "Archivo " << PRESTAMOS_CSV << " no encontrado. Creando archivo vacío." << endl;
            // Crea un archivo vacío para evitar errores posteriores
            guardarPrestamosCSV();
            // Termina la ejecución de la función
            return;
        }
        
        // Variable para leer línea por línea
        string line;
        // Lee y descarta el encabezado
        getline(file, line);
        
        // Itera mientras haya líneas en el archivo
        while (getline(file, line)) {
            // Parsea la línea separando por comas
            vector<string> campos = parse_csv_line(line, DELIMITADOR);
            
            // Verifica que existan al menos los 6 campos requeridos
            // id_prestamo, isbn, id_usuario, nombre_usuario, titulo, activo
            if (campos.size() >= 6) {
                // Crea objeto Prestamo temporal
                Prestamo p;
                // Asigna ID del préstamo (campo 0)
                p.id_prestamo = campos[0];
                // Asigna ISBN (campo 1)
                p.isbn = campos[1];
                // Asigna ID del usuario (campo 2)
                p.id_usuario = campos[2];
                // Convierte el texto "true" o "false" a booleano (campo 5)
                p.activo = (campos[5] == "true");
                
                // Guarda el préstamo en el mapa usando su ID como clave
                prestamos[p.id_prestamo] = p;
            }
        }
        // Cierra el archivo
        file.close();
        // Mensaje de éxito
        cout << "Prestamos cargados desde " << PRESTAMOS_CSV << endl;
    }

    // Guarda los préstamos actuales en el archivo CSV
    void guardarPrestamosCSV() {
        // Abre el archivo para escritura (sobrescribe)
        ofstream file(PRESTAMOS_CSV);
        
        // Verifica errores de apertura
        if (!file.is_open()) {
            // Muestra error en salida estándar de error
            cerr << "Error: No se pudo abrir " << PRESTAMOS_CSV << " para escritura." << endl;
            return;
        }
        
        // Escribe encabezado
        file << "id_prestamo,isbn,id_usuario,nombre_usuario,titulo,activo\n";
        
        // Itera sobre el mapa de préstamos
        for (const auto& pair : prestamos) {
            // Referencia al objeto préstamo
            const Prestamo& p = pair.second;
            
            // Busca el nombre del usuario para guardarlo como referencia legible (desnormalización)
            // Si el usuario ya no existe, pone "NO ENCONTRADO"
            string nombre_usuario = usuarios.count(p.id_usuario) ? usuarios.at(p.id_usuario).nombre : "NO ENCONTRADO";
            
            // Busca el título del libro para referencia legible
            string titulo_libro = libros.count(p.isbn) ? libros.at(p.isbn).titulo : "NO ENCONTRADO";
            
            // Convierte el booleano a string "true"/"false"
            string activo_str = p.activo ? "true" : "false";
            
            // Escribe la línea CSV con todos los campos entrecomillados
            file << csv_quote(p.id_prestamo) << DELIMITADOR
                << csv_quote(p.isbn) << DELIMITADOR
                << csv_quote(p.id_usuario) << DELIMITADOR
                << csv_quote(nombre_usuario) << DELIMITADOR
                << csv_quote(titulo_libro) << DELIMITADOR
                << activo_str << "\n";
        }
        // Cierra el archivo
        file.close();
    }

    // Carga la lista de espera (colas) desde el CSV
    void cargarListaEsperaCSV() {
        // Abre el archivo de lista de espera
        ifstream file(LISTA_ESPERA_CSV);
        
        // Verifica si el archivo existe
        if (!file.is_open()) {
            // Si no existe, avisa y crea uno nuevo
            cout << "Archivo " << LISTA_ESPERA_CSV << " no encontrado. Creando archivo vacío." << endl;
            guardarListaEsperaCSV();
            return; 
        }
        
        // Variable para líneas
        string line;
        // Salta encabezado
        getline(file, line); 
        
        // Lee el archivo línea a línea
        while (getline(file, line)) {
            // Parsea la línea
            vector<string> campos = parse_csv_line(line, DELIMITADOR);
            
            // Se requieren al menos 2 campos: ISBN y la cadena de usuarios
            if (campos.size() >= 2) {
                // ISBN del libro en espera
                string isbn = campos[0];
                // String con los IDs de usuarios separados por '^'
                string cadena_usuarios = campos[1];
                
                // Cola temporal para reconstruir la fila
                queue<string> q;
                // Stream para procesar la cadena de usuarios
                stringstream ss(cadena_usuarios);
                string u;
                
                // Itera sobre los usuarios separados por '^'
                while (getline(ss, u, '^')) { 
                    // Si el ID extraído no está vacío
                    if (!u.empty()) {
                        // Lo empuja a la cola
                        q.push(u);
                    }
                }
                
                // Si la cola recuperada tiene gente, se guarda en el mapa
                if (!q.empty()) {
                    // std::move transfiere la propiedad de 'q' al mapa (eficiente)
                    lista_espera[isbn] = std::move(q);
                }
            }
        }
        // Cierra archivo
        file.close();
        // Mensaje de éxito
        cout << "Lista de espera cargada desde " << LISTA_ESPERA_CSV << endl;
    }

    // Guarda las colas de espera en el CSV
    void guardarListaEsperaCSV() {
        // Abre archivo para escritura
        ofstream file(LISTA_ESPERA_CSV);
        
        // Validación de apertura
        if (!file.is_open()) {
            return;
        }
        
        // Escribe encabezado
        file << "isbn,cola_usuarios\n";
        
        // Itera sobre el mapa de listas de espera
        for (auto& p : lista_espera) {
            // ISBN (clave)
            const string& isbn = p.first;
            // Copia temporal de la cola porque 'std::queue' no es iterable directamente
            // y al hacer pop() la vaciamos, así que necesitamos una copia para no borrar la memoria RAM
            queue<string> q = p.second; 
            
            // Vector para construir la cadena
            vector<string> usuarios_vec;
            
            // Vacía la copia de la cola para llenar el vector
            while (!q.empty()) {
                usuarios_vec.push_back(q.front());
                q.pop();
            }
            
            // Une el vector en un string separado por '^'
            string cadena = join(usuarios_vec, "^");
            
            // Solo guardamos la línea si hay gente esperando
            if (!cadena.empty()) {
                file << csv_quote(isbn) << DELIMITADOR << csv_quote(cadena) << "\n";
            }
        }
        // Cierra archivo
        file.close();
    }

    // Función auxiliar para generar un ID de préstamo único
    string generar_id_prestamo() {
        // Retorna "P" seguido de 8 dígitos aleatorios
        return "P" + generar_id_aleatorio(8);
    }

    // Helper privado para indexar texto en Trie y Mapa de búsqueda
    void indexar_termino(string texto, const string& isbn) {
        // Si el texto está vacío, no hay nada que indexar
        if (texto.empty()) {
            return;
        }
        
        // Itera sobre cada carácter para normalizar (minusculas)
        // CORRECCIÓN: Usar unsigned char para evitar corrupción con tildes/caracteres especiales
        for (auto& c : texto) {
            c = tolower(static_cast<unsigned char>(c));
        }
        
        // Inserta la palabra normalizada en el árbol Trie (para autocompletado)
        trie.insertar(texto);
        
        // Mapea la palabra normalizada al ISBN (para saber qué libro es)
        mapa_busqueda[texto].push_back(isbn);
    }
    // Función privada que ejecuta la reversión de una acción específica
    void deshacer_accion(const Accion& a) { 
        // Estructura de control para decidir qué hacer según el tipo de acción guardada
        switch (a.tipo) {
        
        // CASO: Se había agregado un libro -> Reversión: Eliminar ese libro
        case TipoAccion::AgregarLibro: {
            // Verifica si el libro realmente existe en la base de datos
            if (libros.count(a.id)) {
                // Borrado manual rápido del mapa principal
                // Nota: No usamos quitarLibros() completo para evitar recursión o efectos secundarios no deseados aquí
                libros.erase(a.id);
                
                // Borrar del índice (Título -> ISBN)
                // Iterador para recorrer el mapa de índices
                for (auto it = indice.begin(); it != indice.end(); ) {
                    // Si el ISBN del índice coincide con el ID borrado
                    if (it->second == a.id) {
                        // Borra el elemento y actualiza el iterador al siguiente
                        it = indice.erase(it);
                    }
                    else {
                        // Avanza el iterador normalmente
                        ++it;
                    }
                }
                // Mensaje de feedback
                cout << "Acción revertida: Libro " << a.id << " eliminado." << endl;
                
                // Actualizar el archivo CSV para reflejar el cambio
                guardarLibrosCSV();
            }
            break;
        }

        // CASO: Se había eliminado un libro -> Reversión: Restaurarlo (No implementado por complejidad)
        case TipoAccion::EliminarLibro: {
            cout << "Deshacer 'EliminarLibro' no implementado (requiere guardar todos los datos del libro borrado)." << endl;
            break;
        }

        // CASO: Se había modificado un libro -> Reversión: Restaurar estado anterior (No implementado)
        case TipoAccion::ModificarLibro: {
            cout << "Deshacer 'ModificarLibro' no implementado." << endl;
            break;
        }

        // CASO: Se había agregado un usuario -> Reversión: Eliminar al usuario
        case TipoAccion::AgregarUsuario: {
            // Verifica si el usuario existe
            if (usuarios.count(a.id)) {
                // Lo borra del mapa principal
                usuarios.erase(a.id);
                // Feedback
                cout << "Acción revertida: Usuario " << a.id << " eliminado." << endl;
                
                // Actualizar el archivo CSV
                guardarUsuariosCSV();
            }
            break;
        }

        // CASO: Se había eliminado un usuario -> Reversión: Restaurarlo (No implementado)
        case TipoAccion::EliminarUsuario: {
            cout << "Deshacer 'EliminarUsuario' no implementado." << endl;
            break;
        }

        // CASO: Se había prestado un libro -> Reversión: Cancelar el préstamo y devolver libro
        case TipoAccion::PrestarLibro: {
            // a.id aquí es el ID del PRÉSTAMO, verificamos si existe
            if (prestamos.count(a.id)) {
                // Obtiene una copia del objeto préstamo antes de borrarlo
                Prestamo P = prestamos[a.id];
                
                // 1. Borrar el registro del préstamo del mapa
                prestamos.erase(a.id);
                
                // 2. Recuperar la copia del libro (incrementar stock disponible)
                if (libros.count(P.isbn)) {
                    libros[P.isbn].copias_disponibles += 1;
                }
                
                // 3. Quitar del historial activo del usuario
                if (usuarios.count(P.id_usuario)) {
                    // Borra el ISBN del set de préstamos activos
                    usuarios[P.id_usuario].prestamos_activos.erase(P.isbn);
                    
                    // Decrementa el contador de préstamos activos
                    if (usuarios[P.id_usuario].num_prestamos_activos > 0) {
                        usuarios[P.id_usuario].num_prestamos_activos--;
                    }
                    
                    // Nota: Quitarlo del historial_isbn histórico es complejo porque no sabemos 
                    // si el usuario ya había leído este libro antes en otra ocasión. 
                    // Por seguridad, solo revertimos el estado "activo".
                }
                // Feedback
                cout << "Acción revertida: Préstamo " << a.id << " cancelado." << endl;

                // Actualizar TODOS los CSVs involucrados para mantener consistencia
                guardarLibrosCSV();
                guardarUsuariosCSV();
                guardarPrestamosCSV();
            }
            break;
        }

        // CASO: Se había devuelto un libro -> Reversión: Volver a prestarlo (No implementado totalmente)
        case TipoAccion::DevolverLibro: {
             cout << "Deshacer 'DevolverLibro' no implementado completamente." << endl;
            break;
        }

        // CASO: Se puso a alguien en cola -> Reversión: Sacarlo de la cola
        case TipoAccion::PonerenCola: {
            // a.id es el ISBN del libro
            // a.usuario es el ID del usuario
            if (lista_espera.count(a.id)) {
                // Hay que reconstruir la cola quitando al usuario específico
                // Copia la cola original
                queue<string> original = lista_espera[a.id];
                // Crea una cola nueva vacía
                queue<string> nueva;
                // Bandera para asegurar que solo borramos una instancia (la primera)
                bool eliminado = false;
                
                // Recorre la cola original vaciándola
                while (!original.empty()) {
                    string u = original.front();
                    original.pop();
                    
                    // Si encontramos al usuario y aún no lo hemos eliminado
                    if (!eliminado && u == a.usuario) {
                        eliminado = true; // Lo saltamos (efectivamente borrándolo)
                    } 
                    else {
                        // Si no es él (o ya lo borramos antes), lo pasamos a la nueva cola
                        nueva.push(u);
                    }
                }
                // Reemplaza la cola vieja con la nueva filtrada
                lista_espera[a.id] = nueva;
                
                cout << "Acción revertida: Usuario " << a.usuario << " sacado de la cola de espera." << endl;
                // Guarda cambios en CSV
                guardarListaEsperaCSV();
            }
            break;
        }

        // CASO DEFAULT: Tipo de acción no reconocido
        default: {
            cout << "Acción desconocida." << endl;
            break;
        }
        }
    }

public:
    // Constructor de la clase Biblioteca
    Biblioteca() {
        // Carga los libros desde disco al iniciar
        cargarLibrosCSV();
        // Carga los usuarios desde disco
        cargarUsuariosCSV();
        // Carga los préstamos desde disco
        cargarPrestamosCSV();
        // Carga la lista de espera desde disco
        cargarListaEsperaCSV();
        // Inicializa el grafo de recomendaciones basado en los datos cargados
        inicializarGrafo();
    }
    // ----- Libros -----

    // Función para agregar un nuevo libro al sistema
    bool agregarLibro(const Libro& libro) {
        // Verifica si ya existe un libro con ese ISBN
        if (libros.count(libro.isbn)) {
            // Si existe, retorna falso indicando fallo
            return false;
        }
        
        // Inserta el libro en el mapa principal
        libros[libro.isbn] = libro;
        
        // Actualiza el índice de búsqueda por título
        indice[libro.titulo] = libro.isbn;
        
        // Indexa el título en el Trie y mapa de búsqueda para autocompletado
        indexar_termino(libro.titulo, libro.isbn); 
        
        // Itera sobre cada autor para indexarlo también
        for (const string& autor : libro.autores) {
            indexar_termino(autor, libro.isbn); 
        }
        
        // Si el libro tiene un ISBN numérico válido, lo inserta en el árbol AVL
        if (libro.isbn_num != 0) {
            isbn_avl.insertar(libro.isbn_num, libro.isbn);
        }
        
        // Registra la acción en el historial para poder deshacerla luego
        registrar_accion({ TipoAccion::AgregarLibro, libro.isbn, "", "" });
        
        // Guarda los cambios en el archivo CSV inmediatamente
        guardarLibrosCSV();
        
        // Retorna verdadero indicando éxito
        return true;
    }

    // Función compleja para eliminar un libro y limpiar todos sus rastros
    bool quitarLibros(const string& isbn) {
        // Verifica si el libro existe
        if (!libros.count(isbn)) {
            return false;
        }

        // 1. Limpiar PRÉSTAMOS asociados a este libro
        // Vector auxiliar para guardar los IDs de los préstamos a borrar
        // (No se debe borrar de un mapa mientras se itera sobre él)
        vector<string> prestamos_a_borrar;
        
        // Itera sobre todos los préstamos
        for (auto& par : prestamos) {
            // Si el préstamo corresponde al libro que vamos a borrar
            if (par.second.isbn == isbn) {
                // Guarda el ID para borrarlo después
                prestamos_a_borrar.push_back(par.first);
            }
        }
        
        // Borra los préstamos identificados
        for (const string& pid : prestamos_a_borrar) {
            prestamos.erase(pid);
        }

        // 2. Limpiar USUARIOS (Historiales y Préstamos Activos)
        // Itera sobre todos los usuarios
        for (auto& par_usuario : usuarios) {
            Usuario& u = par_usuario.second;

            // A. Si el usuario lo tiene prestado actualmente, lo quitamos del set activo
            if (u.prestamos_activos.count(isbn)) {
                u.prestamos_activos.erase(isbn);
                // Actualiza el contador de préstamos activos (evita negativos con max)
                u.num_prestamos_activos = max(0, u.num_prestamos_activos - 1);
            }

            // B. Limpiar del historial de ISBNs (vector)
            // Usa el idioma erase-remove: 'remove' mueve los elementos a borrar al final
            // y devuelve el iterador al nuevo final lógico.
            auto it_isbn = remove(u.historial_isbn.begin(), u.historial_isbn.end(), isbn);
            
            // Si se encontraron elementos para borrar
            if (it_isbn != u.historial_isbn.end()) {
                // 'erase' elimina físicamente los elementos del vector
                u.historial_isbn.erase(it_isbn, u.historial_isbn.end());
            }

            // C. Limpiar del historial de Títulos (vector de strings)
            // Nota: Como estamos borrando el libro, necesitamos su título actual antes de que desaparezca
            string titulo_a_borrar = libros[isbn].titulo;
            
            // Busca y elimina el título del historial de lectura del usuario
            auto it_titulo = remove(u.historial_titulos.begin(), u.historial_titulos.end(), titulo_a_borrar);
            
            // Si se encontró, se borra
            if (it_titulo != u.historial_titulos.end()) {
                u.historial_titulos.erase(it_titulo, u.historial_titulos.end());
            }
        }

        // 3. Eliminar del índice auxiliar (Titulo -> ISBN)
        // Itera sobre el mapa de índices
        for (auto it = indice.begin(); it != indice.end(); ) {
            // Si encuentra la entrada que apunta a este ISBN
            if (it->second == isbn) {
                // Borra la entrada y actualiza el iterador
                it = indice.erase(it);
            }
            else {
                // Avanza al siguiente
                ++it;
            }
        }

        // 4. Eliminar el libro del mapa principal
        libros.erase(isbn);

        // 5. GUARDAR TODO (Para mantener sincronía entre archivos)
        guardarLibrosCSV();    // El libro desaparece
        guardarPrestamosCSV(); // Los préstamos huérfanos desaparecen
        guardarUsuariosCSV();  // Los historiales de usuario se limpian
        
        return true;
    }

    // Función para modificar los datos de un libro existente
    bool modificarLibro(const string& isbn, const Libro& nuevo) {
        // Verifica que el libro original exista
        if (!libros.count(isbn)) {
            return false;
        }
        
        // Guarda una copia del libro viejo para comparar cambios
        Libro viejo = libros[isbn];
        string titulo_anterior = viejo.titulo;

        // Actualiza el libro en el mapa principal con los datos nuevos
        libros[isbn] = nuevo;

        // Actualiza el índice secundario
        // Primero busca y borra la entrada antigua en el índice
        for (auto it = indice.begin(); it != indice.end(); ++it) {
            if (it->second == isbn) { 
                indice.erase(it); 
                break; // Solo debería haber una entrada por libro
            }
        }
        // Inserta la nueva entrada en el índice
        indice[nuevo.titulo] = isbn;
        
        // Re-indexa el nuevo título y autores para la búsqueda
        indexar_termino(nuevo.titulo, nuevo.isbn); 
        for (const string& autor : nuevo.autores) {
            indexar_termino(autor, nuevo.isbn); 
        }

        // --- CASCADA: Actualizar título en Usuarios y Préstamos si cambió ---
        if (titulo_anterior != nuevo.titulo) {
            // 1. Actualizar Préstamos activos (que guardan una copia del título)
            for (auto& par : prestamos) {
                if (par.second.isbn == isbn) {
                    par.second.titulo = nuevo.titulo;
                }
            }

            // 2. Actualizar Historial de Usuarios (Aquí es texto plano, hay que buscar y reemplazar)
            for (auto& par_u : usuarios) {
                Usuario& u = par_u.second;
                // Itera por referencia sobre cada título en el historial
                for (string& t : u.historial_titulos) {
                    // Si coincide con el título viejo
                    if (t == titulo_anterior) {
                        // Lo reemplaza con el nuevo
                        t = nuevo.titulo;
                    }
                }
            }
        }
        // ----------------------------------------------------------

        // Guarda cambios en todos los archivos afectados
        guardarLibrosCSV();
        guardarUsuariosCSV(); // Se actualiza por si cambiaron los historiales de títulos
        guardarPrestamosCSV(); // Se actualiza por si cambió el título en el registro de préstamo

        return true;
    }
    // Función para mostrar los detalles de un libro específico en consola
    void mostrar_libro(const string& isbn) const {
        // Verifica si el libro existe en el mapa
        if (!libros.count(isbn)) { 
            // Si no existe, imprime mensaje de error
            cout << "No existe libro con ISBN " << isbn << endl; 
            // Sale de la función
            return; 
        }
        
        // Obtiene una referencia constante al libro (para no modificarlo)
        const Libro& b = libros.at(isbn);
        
        // Imprime los campos básicos
        cout << "ISBN: " << b.isbn << " - Titulo: " << b.titulo << " - Genero: " << b.genero << " - Autores: ";
        
        // Itera sobre el vector de autores para imprimirlos
        for (size_t i = 0; i < b.autores.size(); ++i) {
            // Si no es el primer autor, imprime una coma separadora antes
            if (i) {
                cout << ", ";
            }
            // Imprime el nombre del autor
            cout << b.autores[i];
        }
        
        // Imprime fecha y disponibilidad (Disponibles / Totales)
        cout << " - Fecha: " << b.fecha_publi << " - Copias: " << b.copias_disponibles << "/" << b.copias_totales << endl;
    }

    // Función que utiliza el árbol AVL para mostrar libros ordenados por su ISBN numérico
    void mostrar_libros_por_isbn_num_ordenado() const {
        cout << "--- Libros ordenados por ISBN numerico (via AVL) ---" << endl;
        
        // Obtiene el vector ordenado haciendo un recorrido In-Order del AVL
        vector<pair<long long, string>> isbns_ordenados = isbn_avl.obtenerClavesOrdenadas();
        
        // Verifica si la lista está vacía
        if (isbns_ordenados.empty()) { 
            // Informa al usuario
            cout << "No hay libros con ISBN numerico valido." << endl; 
            return; 
        }
        
        // Itera sobre el vector ordenado
        for (const auto& p : isbns_ordenados) {
            // Imprime el valor numérico de la clave
            cout << "ISBN Num: " << p.first << endl;
            // Llama a mostrar_libro usando el string original del ISBN (p.second)
            mostrar_libro(p.second);
        }
    }

    // Función auxiliar para obtener un puntero a un libro (útil para validaciones externas)
    const Libro* obtenerLibroPorISBN(const string& isbn) const {
        // Busca el elemento en el mapa
        auto it = libros.find(isbn);
        
        // Si el iterador no llegó al final (lo encontró)
        if (it != libros.end()) {
            // Retorna la dirección de memoria del objeto Libro
            return &(it->second);
        }
        // Si no lo encuentra, retorna puntero nulo
        return nullptr;
    }

    // Función de búsqueda flexible por género
    void mostrar_libros_por_genero(const string& genero) const {
        // Copia el género de búsqueda para modificarlo (normalización)
        string genero_busqueda = genero;
        
        // Transforma la cadena de búsqueda a minúsculas
        transform(genero_busqueda.begin(), genero_busqueda.end(), genero_busqueda.begin(),
            [](unsigned char c) { return tolower(c); });
            
        // Elimina caracteres no alfanuméricos de la búsqueda (espacios, símbolos)
        genero_busqueda.erase(remove_if(genero_busqueda.begin(), genero_busqueda.end(),
            [](unsigned char c) { return !isalnum(c); }), genero_busqueda.end());

        cout << "--- Libros del género: " << genero << " ---" << endl;
        
        // Bandera para saber si encontramos al menos uno
        bool encontrado = false;
        
        // Itera sobre TODOS los libros de la biblioteca (Búsqueda lineal)
        for (const auto& par : libros) {
            // Obtiene referencia al libro actual
            const Libro& l = par.second;
            
            // Copia el género del libro actual para normalizarlo
            string l_gen = l.genero;
            
            // Transforma género del libro a minúsculas
            transform(l_gen.begin(), l_gen.end(), l_gen.begin(),
                [](unsigned char c) { return tolower(c); });
                
            // Elimina caracteres no alfanuméricos del género del libro
            l_gen.erase(remove_if(l_gen.begin(), l_gen.end(),
                [](unsigned char c) { return !isalnum(c); }), l_gen.end());
            
            // Verifica si el género buscado es una subcadena del género del libro
            // Esto permite búsquedas parciales (ej: "ficcion" encuentra "Ciencia Ficción")
            if (l_gen.find(genero_busqueda) != string::npos) {
                // Muestra el libro
                mostrar_libro(l.isbn);
                // Marca que encontramos algo
                encontrado = true;
            }
        }
        
        // Si tras revisar todos no encontramos nada
        if (!encontrado) {
            cout << "No se encontraron libros del género " << genero << endl;
        }
    }
    // ----- Usuarios -----

    // Función para registrar un nuevo usuario
    bool agregarUsuario(const Usuario& u) {
        // Verifica si el ID de usuario ya existe
        if (usuarios.count(u.id_usuario)) {
            return false;
        }
        
        // Inserta el usuario en el mapa
        usuarios[u.id_usuario] = u;
        
        // Registra la acción para poder deshacerla
        registrar_accion({ TipoAccion::AgregarUsuario, u.id_usuario, "", "" });
        
        // Guarda en CSV
        guardarUsuariosCSV();
        
        return true;
    }

    // Función para eliminar un usuario y limpiar sus dependencias
    bool eliminarUsuario(const string& uid) {
        // Verifica si el usuario existe
        if (!usuarios.count(uid)) {
            return false;
        }

        // Obtiene referencia al usuario
        Usuario& u = usuarios[uid];

        // 1. Devolver libros que el usuario tenga activos (Recuperar stock para la biblioteca)
        // Itera sobre los ISBNs que tiene prestados actualmente
        for (const string& isbn : u.prestamos_activos) {
            // Si el libro existe en la base de datos
            if (libros.count(isbn)) {
                // Incrementa las copias disponibles (como si los devolviera forzosamente)
                libros[isbn].copias_disponibles++;
            }
        }

        // 2. Eliminar PRÉSTAMOS asociados a este usuario en el archivo de prestamos
        // Vector para recolectar IDs de préstamos a borrar
        vector<string> prestamos_a_borrar;
        
        // Busca en todos los préstamos
        for (auto& par : prestamos) {
            // Si el préstamo pertenece al usuario a eliminar
            if (par.second.id_usuario == uid) {
                prestamos_a_borrar.push_back(par.first);
            }
        }
        
        // Borra los préstamos encontrados
        for (const string& pid : prestamos_a_borrar) {
            prestamos.erase(pid);
        }

        // 3. Eliminar usuario del mapa principal
        usuarios.erase(uid);

        // 4. GUARDAR TODO
        guardarUsuariosCSV();    // Elimina al usuario del archivo
        guardarLibrosCSV();      // Guarda el stock actualizado (libros recuperados)
        guardarPrestamosCSV();   // Elimina los registros de préstamo
        
        return true;
    }

    // ----- Préstamos / Devoluciones -----

    // Función principal para procesar el préstamo de un libro
    // Retorna: 1 (Éxito), 2 (En cola), 0 (Libro no existe), -1 (Usuario no existe)
    int prestamoLibro(const string& id_usuario, const string& isbn) {
        // Validación: Usuario no existe
        if (!usuarios.count(id_usuario)) {
            return -1; 
        }
        // Validación: ISBN no existe
        if (!libros.count(isbn)) {
            return 0;          
        }

        // Obtiene referencia al libro
        Libro& l = libros[isbn];

        // CASO 1: Hay copias disponibles -> Prestar inmediatamente
        if (l.copias_disponibles > 0) {
            // Reduce el inventario disponible
            l.copias_disponibles -= 1;
            
            // Registra el libro en los préstamos activos del usuario
            usuarios[id_usuario].prestamos_activos.insert(isbn);
            
            // Lo añade al historial general de lecturas
            usuarios[id_usuario].historial_isbn.push_back(isbn);
            
            // Incrementa contador de préstamos
            usuarios[id_usuario].num_prestamos_activos++;

            // --- Actualizar GRAFO de conexiones (para recomendaciones) ---
            // Conecta este nuevo libro con todos los libros previos del historial del usuario
            for (const string& otro : usuarios[id_usuario].historial_isbn) {
                // Evita conectarse consigo mismo
                if (otro == isbn) {
                    continue;
                }
                // Incrementa peso bidireccional
                grafico_libro[isbn][otro] += 1;
                grafico_libro[otro][isbn] += 1;
            }

            // Crear objeto Préstamo
            Prestamo P;
            P.id_prestamo = generar_id_prestamo();
            P.isbn = isbn;
            P.id_usuario = id_usuario;
            P.activo = true;
            
            // Guardar préstamo en el mapa
            prestamos[P.id_prestamo] = P;

            // Registrar acción para Undo
            registrar_accion({ TipoAccion::PrestarLibro, P.id_prestamo, id_usuario, isbn });

            // Guardar cambios en todos los archivos
            guardarLibrosCSV();
            guardarPrestamosCSV();
            guardarUsuariosCSV();
            
            return 1; // Código de ÉXITO
        }
        
        // CASO 2: No hay copias -> Añadir a Cola de espera
        else {
            // Añade el ID del usuario a la cola de ese ISBN
            lista_espera[isbn].push(id_usuario);
            
            // Registra acción (para poder sacarlo de la cola si se deshace)
            registrar_accion({ TipoAccion::PonerenCola, isbn, id_usuario, "" });
            
            // Actualiza archivo de cola
            guardarListaEsperaCSV(); 
            
            return 2; // Código de EN COLA
        }
    }

    // Función para procesar la devolución de un libro
    bool devolver_libro(const string& id_usuario, const string& isbn) {
        // Validaciones básicas de existencia
        if (!usuarios.count(id_usuario) || !libros.count(isbn)) {
            return false;
        }

        // 1. Buscar y desactivar el préstamo actual
        string pid_actual = "";
        
        // Busca el préstamo activo específico para este usuario y libro
        for (auto& p : prestamos) {
            // Coincidencia de ISBN, Usuario y que esté Activo
            if (p.second.isbn == isbn && p.second.id_usuario == id_usuario && p.second.activo) {
                pid_actual = p.first;
                break; // Encontrado
            }
        }
        
        // Si no se encontró un préstamo activo correspondiente
        if (pid_actual.empty()) {
            return false;
        }

        // Marca el préstamo como inactivo (finalizado)
        prestamos[pid_actual].activo = false;
        
        // Lo quita de la lista de activos del usuario
        usuarios[id_usuario].prestamos_activos.erase(isbn);
        
        // Decrementa contador
        if (usuarios[id_usuario].num_prestamos_activos > 0) {
            usuarios[id_usuario].num_prestamos_activos--;
        }
        
        // Guarda el título en el historial de títulos (para persistencia visual)
        usuarios[id_usuario].historial_titulos.push_back(libros[isbn].titulo);

        // Registra acción de devolución
        registrar_accion({ TipoAccion::DevolverLibro, pid_actual, id_usuario, isbn });

        // 2. VERIFICAR COLA DE ESPERA (Lógica automática)
        if (!lista_espera[isbn].empty()) {
            // A. Sacar al siguiente usuario de la fila
            string siguiente_usuario = lista_espera[isbn].front();
            lista_espera[isbn].pop();

            // B. Crear préstamo automático para él 
            // IMPORTANTE: No incrementamos 'copias_disponibles' porque el libro pasa de una mano a otra inmediatamente.
            
            if (usuarios.count(siguiente_usuario)) {
                Usuario& u_next = usuarios[siguiente_usuario];
                
                // Actualiza datos del nuevo usuario
                u_next.prestamos_activos.insert(isbn);
                u_next.historial_isbn.push_back(isbn);
                u_next.num_prestamos_activos++;
                
                // Actualiza grafo para el nuevo usuario
                for (const string& otro : u_next.historial_isbn) {
                    if (otro == isbn) {
                        continue;
                    }
                    grafico_libro[isbn][otro] += 1;
                    grafico_libro[otro][isbn] += 1;
                }

                // Genera el nuevo registro de préstamo
                Prestamo P;
                P.id_prestamo = generar_id_prestamo();
                P.isbn = isbn;
                P.id_usuario = siguiente_usuario;
                P.activo = true;
                prestamos[P.id_prestamo] = P;
                
                // Nota: No registramos acción de "Prestar" aquí en el historial de deshacer 
                // para mantener simple el flujo de Undo de la devolución original.
            }
            // Notificación en consola
            cout << ">>> AVISO: El libro ha sido asignado automáticamente a " << siguiente_usuario << " de la lista de espera." << endl;
        } 
        else {
            // Si nadie espera, el libro vuelve al estante
            libros[isbn].copias_disponibles += 1;
        }

        // 3. ACTUALIZAR TODOS LOS CSV
        guardarLibrosCSV();        // Stock
        guardarPrestamosCSV();     // Estado de préstamos
        guardarUsuariosCSV();      // Historiales
        guardarListaEsperaCSV();   // Cola

        return true;
    }
    // ----- Recomendaciones -----

    // Función que genera recomendaciones para un usuario basándose en su historial
    // Utiliza un grafo donde los nodos son libros y las aristas representan lecturas conjuntas
    vector<pair<string, int>> recomendar_para_usuario(const string& id_usuario, int K = 10) {
        // Vector para almacenar los resultados (ISBN, Puntuación)
        vector<pair<string, int>> res;
        
        // 1. Validar existencia del usuario
        if (!usuarios.count(id_usuario)) {
            // Si no existe, retorna lista vacía
            return res;
        }
        
        // 2. Preparar estructuras auxiliares
        // Mapa para acumular puntos de recomendación para cada libro candidato
        unordered_map<string, int> scores;
        
        // Conjunto para acceso rápido a los libros que el usuario YA leyó
        // (para no recomendarle algo que ya conoce)
        unordered_set<string> leidos(usuarios[id_usuario].historial_isbn.begin(), usuarios[id_usuario].historial_isbn.end());
        
        // 3. BARRIDO DEL GRAFO (Algoritmo Colaborativo)
        // Recorre cada libro en el historial de lectura del usuario objetivo
        for (const string& isbn_leido : usuarios[id_usuario].historial_isbn) {
            
            // Verifica si este libro tiene conexiones en el grafo (alguien más lo leyó junto con otros)
            if (grafico_libro.count(isbn_leido)) {
                
                // Itera sobre todos los "vecinos" (libros conectados) en el grafo
                for (auto& par_vecino : grafico_libro[isbn_leido]) {
                    // Obtiene el ISBN del libro vecino (candidato a recomendación)
                    const string& isbn_candidato = par_vecino.first;
                    // Obtiene el peso de la conexión (cuántas veces han sido leídos juntos)
                    int peso_conexion = par_vecino.second;
                    
                    // Si el usuario ya leyó el candidato
                    if (leidos.count(isbn_candidato)) {
                        // Lo saltamos, no es una recomendación válida
                        continue;
                    }
                    
                    // Si es nuevo para el usuario, sumamos el peso al puntaje total del candidato
                    scores[isbn_candidato] += peso_conexion;
                }
            }
        }

        // 4. Pasar del mapa de puntuaciones al vector de resultados para poder ordenar
        for (auto& p : scores) {
            // Empuja el par (ISBN, Score) al vector
            res.push_back({ p.first, p.second });
        }

        // 5. Ordenar por relevancia (Mayor puntaje primero)
        sort(res.begin(), res.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            // Criterio principal: Puntuación (descendente)
            if (a.second != b.second) {
                return a.second > b.second; 
            }
            // Criterio de desempate: ISBN (ascendente) para determinismo
            return a.first < b.first;
        });

        // 6. Recortar a K resultados (Top K)
        if ((int)res.size() > K) {
            // Redimensiona el vector manteniendo solo los primeros K elementos
            res.resize(K);
        }
        
        // Retorna la lista final de recomendaciones
        return res;
    }

    // Esta función busca por Título Y Autor usando el Trie
    // Retorna una lista de strings formateados para mostrar al usuario
    vector<string> autocompletado_general(const string& prefijo, int K = 10) {
        // Vector para almacenar las sugerencias finales
        vector<string> sugerencias_finales;
        
        // 1. Validación de seguridad básica
        if (prefijo.empty()) {
            return sugerencias_finales;
        }

        // 2. Normalizar prefijo a minúsculas de forma SEGURA
        // Creamos copia del prefijo
        string p_low = prefijo;
        // Iteramos por referencia para modificar cada carácter
        for (auto& c : p_low) {
            // Casting a unsigned char evita errores con caracteres extendidos (tildes)
            c = tolower(static_cast<unsigned char>(c));
        }

        // 3. Usar el Trie para obtener terminaciones posibles
        // Busca en el árbol todas las palabras que completan el prefijo dado
        vector<string> coincidencias_trie = trie.buscarPrefijo(p_low);

        // 4. Recuperar títulos reales a partir de las coincidencias del Trie
        // Contador para limitar resultados a K
        int contador = 0;
        // Conjunto para evitar duplicados en la lista de resultados
        unordered_set<string> isbns_vistos; 

        // Itera sobre cada palabra encontrada en el Trie (títulos o autores normalizados)
        for (const string& llave : coincidencias_trie) {
            
            // Busca la llave en el mapa inverso (Texto normalizado -> Lista de ISBNs)
            // Usamos find() para evitar crear entradas vacías accidentalmente con []
            auto it_mapa = mapa_busqueda.find(llave);
            
            // Si la llave existe en el mapa
            if (it_mapa != mapa_busqueda.end()) {
                
                // Itera sobre todos los ISBNs asociados a esa palabra clave
                for (const string& isbn : it_mapa->second) {
                    
                    // Si ya agregamos este libro a la lista de resultados, lo saltamos
                    if (isbns_vistos.count(isbn)) {
                        continue;
                    }
                    
                    // Verificar que el libro siga existiendo (pudo ser borrado de la biblioteca)
                    auto it_libro = libros.find(isbn);
                    
                    // Si el libro existe
                    if (it_libro != libros.end()) {
                        // Referencia al libro
                        const Libro& l = it_libro->second;
                        
                        // Construir string de presentación: Título + Autores
                        string autores_str = join(l.autores, ", ");
                        string display = l.titulo + " (Autor: " + autores_str + ")";
                        
                        // Añade a la lista de resultados
                        sugerencias_finales.push_back(display);
                        // Marca el ISBN como visto
                        isbns_vistos.insert(isbn);
                        // Incrementa contador
                        contador++;
                    }
                    
                    // Si alcanzamos el límite K, terminamos la búsqueda
                    if (contador >= K) {
                        return sugerencias_finales;
                    }
                }
            }
        }
        // Retorna las sugerencias encontradas
        return sugerencias_finales;
    }
    // ----- Mostrar usuario / titulos -----
    
    // Función para mostrar toda la información de un usuario
    void mostrar_usuario(const string& uid) {
        // 1. Verificar existencia de forma segura buscando en el mapa
        auto it_usuario = usuarios.find(uid);
        
        // Si el iterador llega al final, el usuario no existe
        if (it_usuario == usuarios.end()) { 
            cout << "No existe usuario con ID: " << uid << endl; 
            return; 
        }

        // 2. Acceder usando el iterador (más seguro que .at() tras corrupción potencial)
        const Usuario& u = it_usuario->second;

        // Imprime datos básicos
        cout << "Usuario: " << u.id_usuario << " - " << u.nombre << " (" << u.correo << ")\n";
        
        // Imprime lista de libros prestados actualmente
        cout << "Prestados (ISBNs activos): ";
        
        // Verifica si la lista está vacía
        if (u.prestamos_activos.empty()) {
            cout << "(Ninguno)";
        } 
        else {
            // Itera sobre el conjunto de préstamos activos
            for (const string& isbn : u.prestamos_activos) {
                cout << isbn << " ";
            }
        }
        
        // Imprime historial completo de lecturas (Títulos)
        cout << "\nHistorial de lecturas (Títulos): ";
        
        // Verifica si el historial está vacío
        if (u.historial_isbn.empty()) {
            cout << "(Vacío)";
        } 
        else {
            // Itera sobre el historial de ISBNs
            for (const string& isbn : u.historial_isbn) {
                // Verificación doble para evitar crashes si el libro fue borrado de la BD
                auto it_libro = libros.find(isbn);
                
                // Si el libro aún existe en el sistema
                if (it_libro != libros.end()) {
                    cout << "[" << it_libro->second.titulo << "] ";
                } 
                else {
                    // Si el libro fue eliminado, mostramos el ISBN huérfano
                    cout << "[Libro eliminado (" << isbn << ")] ";
                }
            }
        }
        // Salto de línea final
        cout << endl;
    }

    // Muestra todos los títulos ordenados alfabéticamente
    void mostrar_titulos_ordenados() {
        cout << "\nTitulos ordenados: \n" << endl;
        
        // Itera sobre el mapa 'indice' que, al ser un std::map, mantiene las claves (títulos) ordenadas
        for (auto& p : indice) {
            // Imprime Título (p.first) e ISBN (p.second)
            cout << p.first << " (" << p.second << ")" << endl;
        }
    }

    // Función pública para deshacer la última acción registrada
    void deshacerUltimaOperacion() {
        // Verifica si la pila de acciones está vacía
        if (historial_acciones.empty()) {
            cout << "No hay acciones para deshacer." << endl;
            return;
        }
        
        // Tomamos la última acción del vector (simulando top de una pila)
        Accion ultima = historial_acciones.back();
        
        // La sacamos de la pila (pop)
        historial_acciones.pop_back(); 
        
        // Llamamos a la función interna que ejecuta la lógica inversa
        deshacer_accion(ultima); 
    }
};

// ------------------ MAIN -----------------
int main() {
    // Configurar locale del sistema para soportar tildes y caracteres especiales
    setlocale(LC_ALL, "");
    
    // Instancia principal de la clase Biblioteca (carga los CSVs en el constructor)
    Biblioteca B;

    // Definición del menú de opciones como string constante
    string menu = "\nBienvenido a la Biblioteca Inteligente \n------------------------------------- \n1. Agregar libro \n2. Eliminar libro \n3. Modificar libro \n4. Agregar usuario \n5. Eliminar usuario \n6. Prestar libro \n7. Devolver libro \n8. Buscar titulo (Autocompletar) \n9. Listar libros (Ordenado por título) \n10. Recomendar libros \n11. Ver libro \n12. Ver usuario \n13. Listar libros por ISBN Numerico (AVL) \n14. Deshacer la última acción \n15. Buscar por género \n16. Salir\n";

    // Variable para almacenar la opción seleccionada por el usuario
    int opcion;
    
    // Bucle infinito del menú principal
    while (true) {
        // Muestra el menú
        cout << menu;
        cout << "\nSeleccione una opcion: ";
        
        // Intenta leer un entero. Si falla (por ejemplo, usuario escribe letras), entra al if
        if (!(cin >> opcion)) {
            // Rompe el bucle y termina el programa (o podría limpiar el buffer y continuar)
            break;
        }
        
        // Limpia el buffer de entrada hasta el salto de línea para evitar problemas con getline posteriores
        cin.ignore(10000, '\n');
        
        // Condición de salida explicita
        if (opcion == 16) {
            break;
        }
        
        // Estructura switch para manejar las opciones
        switch (opcion) {
            
            // CASO 1: AGREGAR LIBRO
            case 1: {
                // Instancia temporal de Libro
                Libro nb;
                cout << "ISBN (Presione doble ENTER para generar uno automático 978...): ";
                
                // Usamos getline para permitir entrada vacía (Enter)
                string entrada_isbn;
                getline(cin, entrada_isbn);

                // Limpiar espacios por si acaso (Trim)
                entrada_isbn = trim(entrada_isbn);

                // Si el usuario no escribió nada (solo Enter)
                if (entrada_isbn.empty()) {
                    // TRUCO: Genera un ISBN ficticio "978" + 10 dígitos aleatorios
                    nb.isbn = "978" + generar_id_aleatorio(10);
                    cout << ">>> ISBN generado automáticamente: " << nb.isbn << endl;
                } 
                else {
                    // Usa el ISBN proporcionado
                    nb.isbn = entrada_isbn;
                }

                // --- Lógica para el árbol AVL (Conversión a numérico) ---
                string solo_numeros = nb.isbn;
                
                // Eliminamos guiones o letras de la copia del ISBN
                solo_numeros.erase(remove_if(solo_numeros.begin(), solo_numeros.end(), 
                    [](unsigned char c) { return !isdigit(c); }), solo_numeros.end());
                
                // Intenta convertir a long long
                try {
                    // Si quedaron números, convierte
                    if (!solo_numeros.empty()) {
                        nb.isbn_num = stoll(solo_numeros);
                    }
                    else {
                        nb.isbn_num = 0;
                    }
                } 
                catch (...) {
                    // Si falla la conversión (ej. overflow), asigna 0
                    nb.isbn_num = 0;
                }
                // (Nota: cin.ignore() aquí podría sobrar si ya hicimos getline arriba, pero se mantiene por seguridad)
                // cin.ignore(); <--- Comentado porque getline ya consumió el enter y no hubo cin>> antes inmediato

                cout << "Titulo: ";
                getline(cin, nb.titulo);
                
                // (Opcional) Forzar primera letra mayúscula para mejor ordenamiento visual
                if (!nb.titulo.empty()) {
                    nb.titulo[0] = toupper(nb.titulo[0]);
                }

                cout << "Autor(es) (separados por ;): ";
                string a;
                getline(cin, a);
                
                // Limpia vector de autores
                nb.autores.clear();
                
                // Bloque para parsear autores
                {
                    stringstream ss_a(a);
                    string t;
                    // Separa por punto y coma
                    while (getline(ss_a, t, ';')) {
                        // Si el segmento no está vacío
                        if (!t.empty()) {
                            // Trim básico (opcional pero recomendado) y push
                            nb.autores.push_back(t);
                        }
                    }
                }
                
                cout << "Genero: ";
                getline(cin, nb.genero);

                // Variables para la fecha
                int anio, mes, dia;
                
                // 1. Pedir Año con validación
                cout << "Año de publicación: ";
                while (!(cin >> anio)) { 
                    // Si no es número, limpia error y buffer
                    cin.clear(); 
                    cin.ignore(1000, '\n');
                    cout << "Por favor ingrese un número válido para el año: ";
                }

                // 2. Pedir Mes con validación de rango
                cout << "Mes de publicación: ";
                while (!(cin >> mes) || mes < 1 || mes > 12) {
                    cin.clear(); 
                    cin.ignore(1000, '\n');
                    cout << "Mes inválido. Ingrese entre 1 y 12: ";
                }

                // 3. Pedir Día con validación de rango simple
                cout << "Día de publicación: ";
                while (!(cin >> dia) || dia < 1 || dia > 31) {
                    cin.clear(); 
                    cin.ignore(1000, '\n');
                    cout << "Día inválido. Ingrese entre 1 y 31: ";
                }

                // 4. Formatear a String YYYY-MM-DD
                // Agrega '0' a la izquierda si es menor a 10 para mantener formato ISO
                string s_mes = (mes < 10 ? "0" : "") + to_string(mes);
                string s_dia = (dia < 10 ? "0" : "") + to_string(dia);
                
                // Construye la fecha final
                nb.fecha_publi = to_string(anio) + "-" + s_mes + "-" + s_dia;

                cout << "Copias totales: ";
                cin >> nb.copias_totales;
                
                // Inicialmente disponibles = totales
                nb.copias_disponibles = nb.copias_totales;
                
                // Intenta agregar a la biblioteca
                if (B.agregarLibro(nb)) {
                    cout << "Libro agregado y guardado en CSV." << endl;
                }
                else {
                    cout << "ISBN ya existe." << endl;
                }
            } break;
            // CASO 2: ELIMINAR LIBRO
            case 2: {
                string isbn;
                cout << "ISBN: ";
                cin >> isbn;
                
                // Intenta quitar el libro usando la función de la clase
                if (B.quitarLibros(isbn)) {
                    cout << "Libro removido y CSV actualizado." << endl;
                }
                else {
                    cout << "No existe." << endl;
                }
            } break;

            // CASO 3: MODIFICAR LIBRO
            case 3: {
                string isbn;
                cout << "ISBN a modificar: ";
                cin >> isbn;
                // Limpiar buffer
                cin.ignore();
                
                // Validación básica
                if (isbn.empty()){
                    cout << "Se necesita un ISBN para modificar" << endl;
                    break;
                }
                
                // Objeto temporal con los nuevos datos
                Libro nb;
                nb.isbn = isbn;
                
                cout << "Nuevo titulo: ";
                getline(cin, nb.titulo);
                
                cout << "Autor(es) (separados con ;): ";
                string a;
                getline(cin, a);
                
                // Limpia y parsea autores
                nb.autores.clear();
                {
                    stringstream ss_a(a);
                    string t;
                    while (getline(ss_a, t, ';')) {
                        if (!t.empty()) {
                            nb.autores.push_back(t);
                        }
                    }
                }
                
                cout << "Nuevo Genero: ";
                getline(cin, nb.genero);

                // Variables para fecha
                int anio, mes, dia;
                
                // Validación de Año
                cout << "Año de publicación: ";
                while (!(cin >> anio)) { 
                    cin.clear(); 
                    cin.ignore(1000, '\n'); 
                    cout << "Año numérico: "; 
                }
                
                // Validación de Mes
                cout << "Mes de publicación (1-12): ";
                while (!(cin >> mes) || mes < 1 || mes > 12) { 
                    cin.clear(); 
                    cin.ignore(1000, '\n'); 
                    cout << "Mes (1-12): "; 
                }
                
                // Validación de Día
                cout << "Día de publicación (1-31): ";
                while (!(cin >> dia) || dia < 1 || dia > 31) { 
                    cin.clear(); 
                    cin.ignore(1000, '\n'); 
                    cout << "Día (1-31): "; 
                }

                // Formateo de fecha
                string s_mes = (mes < 10 ? "0" : "") + to_string(mes);
                string s_dia = (dia < 10 ? "0" : "") + to_string(dia);
                nb.fecha_publi = to_string(anio) + "-" + s_mes + "-" + s_dia;
                
                cout << "Copias totales: ";
                cin >> nb.copias_totales;
                
                // Actualiza copias disponibles (asumiendo reset o lógica simple)
                // Usamos max(0, ...) para evitar números negativos
                nb.copias_disponibles = max(0, nb.copias_totales);
                
                // Llama a la función de modificación
                if (B.modificarLibro(isbn, nb)) {
                    cout << "Libro modificado y CSV actualizado." << endl;
                }
                else{
                    cout << "No se pudo modificar." << endl;
                }
            } break;

            // CASO 4: AGREGAR USUARIO
            case 4: {
                Usuario nu;
                // Genera ID aleatorio de 12 dígitos
                nu.id_usuario = generar_id_aleatorio(12);
                cout << "ID del usuario generado: " << nu.id_usuario << endl;
                
                cout << "Nombre: ";
                getline(cin, nu.nombre);
                
                cout << "Correo: ";
                getline(cin, nu.correo);
                
                // Intenta agregar usuario
                if (B.agregarUsuario(nu)) {
                    cout << "Usuario agregado con ID: " << nu.id_usuario << " y guardado en CSV." << endl;
                }
                else {
                    cout << "Error: ID generado ya existe (Intente de nuevo)." << endl;
                }
            } break;

            // CASO 5: ELIMINAR USUARIO
            case 5: {
                string uid;
                cout << "ID de usuario: ";
                cin >> uid;
                
                // Llama a eliminar
                if (B.eliminarUsuario(uid)) {
                    cout << "Usuario eliminado y CSV actualizado." << endl;
                }
                else {
                    cout << "No existe ese usuario." << endl;
                }
            } break;

            // CASO 6: PRESTAR LIBRO
            case 6: {
                string uid, isbn;
                cout << "ID de usuario: ";
                cin >> uid;
                cout << "ISBN: ";
                cin >> isbn;
                
                // Llama a prestamoLibro y recoge el código de resultado
                int resultado = B.prestamoLibro(uid, isbn);
                
                // Procesa el resultado
                if (resultado == 1) {
                    cout << "Préstamo realizado exitosamente." << endl;
                }
                else if (resultado == 2) {
                    cout << "No hay copias disponibles. El usuario ha sido agregado a la lista de espera." << endl;
                }
                else if (resultado == 0) {
                    cout << "Error: El ISBN ingresado NO EXISTE en la biblioteca." << endl;
                }
                else if (resultado == -1) {
                    cout << "Error: El Usuario ingresado no existe." << endl;
                }
            } break;

            // CASO 7: DEVOLVER LIBRO
            case 7: {
                string uid, isbn;
                cout << "ID de usuario: ";
                cin >> uid;
                cout << "ISBN: ";
                cin >> isbn;
                
                // Intenta devolver
                if (B.devolver_libro(uid, isbn)) {
                    cout << "Devolucion procesada. CSVs actualizados." << endl;
                }
                else {
                    cout << "Error al devolver. Verifique ID/ISBN." << endl;
                }
            } break;

            // CASO 8: BUSCAR TÍTULO (AUTOCOMPLETAR)
            case 8: {
                string pref;
                cout << "Prefijo de busqueda: ";
                getline(cin, pref);
                
                // 1. Limpiar espacios accidentales (Trim)
                pref = trim(pref);

                // 2. Validar que no esté vacío antes de buscar
                if (pref.empty()) {
                    cout << "Busqueda vacia. Intente de nuevo." << endl;
                } 
                else {
                    // Busca sugerencias usando el Trie
                    auto res = B.autocompletado_general(pref, 10);
                    
                    if (res.empty()) {
                        cout << "No se encontraron coincidencias para '" << pref << "'" << endl;
                    } 
                    else {
                        cout << "Sugerencias: " << endl;
                        for (auto& t : res) {
                            cout << " - " << t << endl;
                        }
                    }
                }
            } break;

            // CASO 9: LISTAR LIBROS (ORDEN ALFABÉTICO)
            case 9: {
                B.mostrar_titulos_ordenados();
            } break;
            
            // CASO 10: RECOMENDAR LIBROS
            case 10: {
                string uid;
                cout << "Ingrese su ID de Usuario: ";
                cin >> uid;
                
                // Llamada al sistema de recomendación (Top 5)
                auto recomendaciones = B.recomendar_para_usuario(uid, 5); 
                
                cout << "--- Libros recomendados para " << uid << " ---" << endl;
                
                if (recomendaciones.empty()) {
                    cout << "No hay suficientes datos para generar recomendaciones." << endl;
                    cout << "(Intente que varios usuarios lean los mismos libros para crear conexiones)" << endl;
                } 
                else {
                    for (auto& p : recomendaciones) {
                        cout << "* ISBN: " << p.first << " (Coincidencia: " << p.second << ")" << endl;
                        // Muestra detalles del libro recomendado
                        B.mostrar_libro(p.first);
                        cout << "--------------------------------" << endl;
                    }
                }
            } break;
            
            // CASO 11: VER LIBRO INDIVIDUAL
            case 11: {
                string isbn;
                cout << "ISBN: ";
                cin >> isbn;
                B.mostrar_libro(isbn);
            } break;

            // CASO 12: VER USUARIO INDIVIDUAL
            case 12: {
                string uid;
                cout << "ID de usuario: ";
                cin >> uid;
                B.mostrar_usuario(uid);
            } break;

            // CASO 13: LISTAR POR ISBN NUMÉRICO (AVL)
            case 13: {
                B.mostrar_libros_por_isbn_num_ordenado();
            } break;

            // CASO 14: DESHACER (UNDO)
            case 14: {
                B.deshacerUltimaOperacion();
            } break;

            // CASO 15: BUSCAR POR GÉNERO
            case 15: {
                string genero;
                cout << "Ingrese el género que desea buscar: ";
                getline(cin, genero);
                B.mostrar_libros_por_genero(genero);
            } break;

            // OPCIÓN POR DEFECTO
            default: {
                cout << "Opción inválida, seleccione algo más por favor" << endl;
            } break;
        }
    }
    // Fin del programa
    cout << "Saliendo..." << endl;
    return 0;
}