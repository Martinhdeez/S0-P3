#include "command_handler.h"

int Cmd_authors(char *tokens[])
{
    struct author
    {
        const char *name;
        const char *login;
    };
    // Inicializamos los datos de los autores
    struct author authors[] = {
        {"Daniel Quinteiro Graña", "daniel.quinteirog@udc.es"},
        {"Martín Hernández González", "m.hernandezg@udc.es"}};
    // Verificamos si el comando tiene parámetros
    if (tokens[1] != NULL)
    {
        if (strcmp(tokens[1], "-l") == 0)
            printf("%s\n%s\n", authors[0].login, authors[1].login);
        else if (strcmp(tokens[1], "-n") == 0)
            printf("%s\n%s\n", authors[0].name, authors[1].name);
        else
        {
            printf("%s: Invalid option -%s\nYou can try help -%s for more informatión\n", tokens[0], tokens[1], tokens[0]);
            return 1; // Retorna 1 si hay una opción invalida
        }
    }
    else
    {
        printf("%s: %s\n", authors[0].name, authors[0].login);
        printf("%s: %s\n", authors[1].name, authors[1].login);
    }
    return 0; // Retorna 0 si todo se ejecuto correctamente
}

int Cmd_pid()
{
    printf("Shell PID: %d\n", getpid());
    return 0;
}

int Cmd_ppid()
{
    printf("Parent PID: %d\n", getppid());
    return 0;
}

int Cmd_cd(char **tokens)
{

    if (tokens[1] == NULL)
    {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("Current directory: %s\n", cwd);
        return 0;
    }

    if (chdir(tokens[1]) != 0)
    {
        perror("cd failed");
        return 1;
    }
    return 0;
}

int Cmd_date(char **tokens)
{
    char current_date[11];
    char current_time[9];
    time_t rawtime = time(NULL); // tiempo desde el 1 de enero de 1970

    struct tm *timeinfo;
    timeinfo = localtime(&rawtime); // tiempo local de mi sistema

    if (tokens[1] == NULL)
    {
        strftime(current_date, sizeof(current_date), "%d/%m/%Y", timeinfo); //
        strftime(current_time, sizeof(current_time), "%H:%M:%S", timeinfo);
        printf("Current date: %s\n", current_date);
        printf("Current time: %s\n", current_time);
        return 0;
    }
    else if (strcmp(tokens[1], "-t") == 0)
    {
        strftime(current_time, sizeof(current_time), "%H:%M:%S", timeinfo);
        printf("Current time: %s\n", current_time);
        return 0;
    }
    else if (strcmp(tokens[1], "-d") == 0)
    {
        strftime(current_date, sizeof(current_date), "%d/%m/%Y", timeinfo);
        printf("Current date: %s\n", current_date);
        return 0;
    }
    printf("Invalid option for date command\n");
    return 1;
}

int isInteger(const char *str)
{
    // Verificar si la cadena es NULL o está vacía
    if (str == NULL || str[0] == '\0')
    {
        return 0; // 0 si no es un número
    }
    int ctr = 0;
    // Si empieza con un signo negativo
    if (str[0] == '-')
    {
        // Si solo contiene '-', no es un número
        if (str[1] == '\0')
            return 0;
        ctr = 2;
    }
    else
    {
        if (!isdigit(str[0]))
            return 0;
        ctr = 1;
    }
    // Verificar que el resto sean dígitos
    for (int i = 1; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
            return 0;
    }
    return ctr; // 2 si es un número negativo y 1
}

void printReverse(struct node *Node, int order, int n)
{
    if (Node == NULL || order < 1 || n < 1)
        return;
    printReverse(Node->next, order - 1, --n);
    printf("%d: %s\n", order, (char *)Node->data);
}

int Cmd_historic(char **tokens)
{
    int ctr = isInteger(tokens[1]);
    int size = getSize(commandHistory);

    if (tokens[1] == NULL)
    { // print all list
        printReverse(first(commandHistory), getSize(commandHistory), getSize(commandHistory));
        return 0;
    }
    else if (ctr == 2)
    { // case: historic -N

        int n = abs(atoi(tokens[1]));
        int order = size;
        if (n > size)
            n = size;

        printReverse(first(commandHistory), order, n);
        return 0;
    }
    else if (ctr == 1)
    { // case historic N

        int n = atoi(tokens[1]);
        if (n > size)
        {
            printf("Command %d not found\n", n);
            return 1;
        }
        int cnt = getSize(commandHistory);

        for (Node *current = first(commandHistory); current != NULL; current = next(current))
        {
            if (cnt == n)
            {
                splitLine(getData(current), tokens);
                if(strcmp(tokens[0], "historic") == 0 && isInteger(tokens[1])){
                    printf("Error, you can't do a historic N of a historic N\n");
                    break;
                } 

                processPrompt(getData(current), tokens);
                break;
            }
            cnt--;
        }
        return 0;
    }
    else
    {
        printf("%s: Invalid option -%s\nYou can try help -%s for more informatión\n", tokens[0], tokens[1], tokens[0]);
        return 1;
    }
}

const char *getModeString(int mode)
{
    static char buffer[100]; // Buffer para almacenar el modo como cadena
    buffer[0] = '\0';        // Inicializa el buffer vacío

    // Verifica cada modo y lo agrega al buffer
    if (mode & O_RDWR)
    {
        strcat(buffer, "O_RDWR ");
    }
    else if (mode & O_WRONLY)
    {
        strcat(buffer, "O_WRONLY ");
    }
    else
    {
        strcat(buffer, "O_RDONLY ");
    }

    if (mode & O_CREAT)
    {
        strcat(buffer, "O_CREAT ");
    }
    if (mode & O_APPEND)
    {
        strcat(buffer, "O_APPEND ");
    }
    if (mode & O_TRUNC)
    {
        strcat(buffer, "O_TRUNC ");
    }
    if (mode & O_EXCL)
    {
        strcat(buffer, "O_EXCL");
    }
    return buffer; // Retorna los modos acumulativos como cadena
}



void printOpenReverse(struct node *Node, int n)
{
    if (Node == NULL || n < 1) return;
    printOpenReverse(Node->next, --n);
    OpenFile *fileInfo = (OpenFile*) getData(Node);
    printf("%10d\t%20s", fileInfo->fd, fileInfo->name);
    if(fileInfo->type == OPEN){
        printf("\t\t    ");
    }else if(fileInfo->type == DUPLICATED){
        printf("(duplicated %d)  ", fileInfo->dupfd);
    }else if(fileInfo->type == MAPPED ){
        printf("(mapping)\t    ");
    }

    if(fileInfo->offset >= 0) printf("%15d\t  %33s\n", fileInfo->offset , getModeString(fileInfo->mode));
    else printf("\t\t\t  %33s\n", getModeString(fileInfo->mode));
}
int Cmd_open(char **tokens)
{
    int i, fd, mode = 0;
    // Verifica si openFileList está vacío
    if (first(openList) == NULL)
    {
        // Agrega la entrada estándar, salida estándar y error estándar a la lista de archivos abiertos
        for (int j = 0; j < 3; j++)
        {                             // Recorre stdin (0), stdout (1), stderr (2)
            mode = fcntl(j, F_GETFL); // Obtiene el modo del descriptor de archivo
            if (mode == -1)
            {
                perror("Error obtainig the file descriptor: ");
                return 0;
            }
            char *name = (j == 0) ? "stdin" : (j == 1) ? "stdout"
                                                       : "stderr";

            OpenFile *file = malloc(sizeof(OpenFile));

            file->name = name;
            file->fd = j;
            file->mode = mode;
            file->offset = -1;
            file->type = OPEN;

            insertElement(file, sizeof(OpenFile), openList); // Agrega a la lista de archivos abiertos
        }
        return 0;
    }

    // Si no se proporcionan tokens adicionales, imprime los archivos abiertos
    if (tokens[1] == NULL)
    {
        Node *current = first(openList);
        printf("\n%10s\t %19s\t %26s\t %33s\n", "Descriptor", "File", "Ofset","Mode");
        printf("%10s\t %19s\t %26s\t %33s\n", "----------", "----", "-----","----");
        printOpenReverse(current, sizeof(openList));
        printf("\n");

        return 0; 
    }

    // Procesa tokens adicionales para abrir modos de archivo
    for (i = 2; tokens[i] != NULL; i++)
    {
        if (!strcmp(tokens[i], "cr"))
            mode |= O_CREAT;
        else if (!strcmp(tokens[i], "ex"))
            mode |= O_EXCL; // se utiliza junto con O_CREAT para garantizar que un archivo nuevo no sobreescriba uno que ya exista
        else if (!strcmp(tokens[i], "ro"))
            mode |= O_RDONLY; // solo lectura
        else if (!strcmp(tokens[i], "wo"))
            mode |= O_WRONLY; // solo escritura
        else if (!strcmp(tokens[i], "rw"))
            mode |= O_RDWR; // Escritura y lectura
        else if (!strcmp(tokens[i], "ap"))
            mode |= O_APPEND; // abrir un archivo de manera que todas las operaciones de escritura que se quieran hacer dentro de el se realizarán                                                   //al FINAL del archivo
        else if (!strcmp(tokens[i], "tr"))
            mode |= O_TRUNC; // si el archivo existe ELIMINA todo su contenido, reduce su tamaño a 0
        else
        {
            printf("Please write a valid command, help open for more information\n");
            return 0;
        }
    }
    // Intenta abrir el archivo y asi evita errores con las limitaciones de los modos
    if ((fd = open(tokens[1], mode, 0777)) == -1)
    {
        perror("Error opening file: ");
        return 1;
    }
    char *name = strdup(tokens[1]);
    if (name == NULL)
    {
        perror("Error duoplicating the filename: ");
        return 1;
    }
    // crear archivo
    OpenFile *file = malloc(sizeof(OpenFile));
    file->name = name;
    file->fd = fd;
    file->mode = mode;
    file->offset = 0;
    file->type = OPEN;

    insertElement(file, sizeof(OpenFile), openList);
    printf("File opened successfuly\n");
    return 0;
}

int Cmd_close(char **tokens)
{
    if (tokens[1] == NULL)
    {
        printf("Error: Missing file descriptor (fd).\n");
        return 1;
    }

    if (openList == NULL)
    {
        printf("Error: Open file list is empty.\n");
        return 1;
    }

    if (!isInteger(tokens[1]))
    {
        printf("Error: The fd must be an integer.\n");
        return 1;
    }

    int fd = atoi(tokens[1]);

    if (fd < 0)
    {
        printf("Error: Invalid file descriptor.\n");
        return 1;
    }

    if (close(fd) == -1)
    {
        perror("Error closing file descriptor\n");
        return 1;
    }
    if (removeElement(&fd, FD, openList))
    {
        printf("File descriptor %d closed successfully.\n", fd);
        return 0;
    }
    else
    {
        printf("Error: File descriptor %d not found in open file list.\n", fd);
        return 1;
    }
}

int Cmd_dup(char **tokens)
{
    if (tokens[1] == NULL || !isInteger(tokens[1]))
    {
        printf("Command not found , dup needs a file descriptor");
        return 1;
    }
    int fd = atoi(tokens[1]);

    Node *current = first(openList);

    while (current != NULL)
    {
        if (current->data != NULL)
        {

            OpenFile *file = getData(current); // coje los datos del nodo

            if (file->fd == fd)
            {
                int newFd = dup(fd);
                if (newFd == -1)
                {
                    perror("Error opening file");
                    return 1;
                }

                OpenFile *newFile = malloc(sizeof(OpenFile));
                if (newFile == NULL)
                {
                    printf("Error allocating memory for the new file");
                    free(newFile);
                    return 1;
                }

                newFile->name = strdup(file->name);
                if (newFile->name == NULL)
                {
                    printf("Error duplicating the file name");
                    free(newFile);
                    return 1;
                }

                newFile->mode = file->mode;
                newFile->fd = newFd;
                newFile->dupfd = fd;
                newFile->offset = 0;
                newFile->type = DUPLICATED;

                insertElement(newFile, sizeof(OpenFile), openList);
                return 0;
            }
            current = next(current);
        }
    }
    printf("File not found\n");
    return 1;
}

int Cmd_infosys()
{

    struct utsname sysinfo; // Unix Time sharing Sistem Name
    // uname obtiene la informacion del sistema
    if (uname(&sysinfo) == -1)
    {
        perror("uname failed");
        return 1;
    }
    // Print system information
    printf("Operating System: %s\n", sysinfo.sysname);
    printf("Node Name: %s\n", sysinfo.nodename);
    printf("System Version: %s\n", sysinfo.release);
    printf("Kernel Version: %s\n", sysinfo.version);
    printf("Architecture: %s\n", sysinfo.machine);
    return 0;
}

char *GetCwd()
{
    char *cwd = malloc(MAX_PROMPT);
    if (cwd == NULL)
    {
        perror("Memory allocation error: ");
        return NULL;
    }

    if (getcwd(cwd, MAX_PROMPT) == NULL)
    {
        perror("getcwd error: ");
        free(cwd);
        return NULL;
    }
    return cwd;
}

int Cmd_cwd()
{
    char *cwd = GetCwd();
    printf("Current work directory: %s\n", cwd);
    free(cwd);
    return 0;
}

int Cmd_makefile(char *tokens[])
{
    if (tokens[1] != NULL)
    {
        int fd = open(tokens[1], O_CREAT | O_EXCL | O_RDWR, 0644);
        if (fd == -1)
        {
            if (errno == EEXIST)
            {
                printf("Error: El archivo '%s' ya existe.\n", tokens[1]);
            }
            else
            {
                perror("Error opening file"); // Otro error
            }
            return 1;
        }
        printf("File created successfully\n");
        close(fd);
    }
    else
    {
        Cmd_cwd();
    }
    return 0;
}

int Cmd_makedir(char **tokens)
{

    char path[MAX_PROMPT];
    char *cwd = GetCwd();

    if (tokens[1] == NULL)
        printf("Current directory: %s\n", cwd);
    else
    {
        snprintf(path, sizeof(path), "%s/%s", cwd, tokens[1]);
        if (mkdir(path, 0755) != 0)
        {
            perror("mkdir error: ");
            free(cwd);
            return 1;
        }
        printf("Directory created: %s\n", path);
    }
    free(cwd);
    return 0;
}

void printPermissions(mode_t mode)
{
    char perms[11];

    // Tipo de archivo (directorio, archivo regular, etc.)
    perms[0] = (S_ISDIR(mode)) ? 'd' : 
               (S_ISLNK(mode)) ? 'l' :
               (S_ISCHR(mode)) ? 'c' :
               (S_ISBLK(mode)) ? 'b' :
               (S_ISFIFO(mode)) ? 'p' :
               (S_ISSOCK(mode)) ? 's' : '-';

    // Permisos del usuario
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? ((mode & S_ISUID) ? 's' : 'x') : ((mode & S_ISUID) ? 'S' : '-');

    // Permisos del grupo
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? ((mode & S_ISGID) ? 's' : 'x') : ((mode & S_ISGID) ? 'S' : '-');

    // Permisos para otros
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? ((mode & S_ISVTX) ? 't' : 'x') : ((mode & S_ISVTX) ? 'T' : '-');

    // Cierre de cadena
    perms[10] = '\0';
    printf("%s ", perms);
}

void printGroupName(gid_t gid)
{
    struct group *grp = getgrgid(gid);
   
    if (grp != NULL)
    {
        printf("%s ", grp->gr_name);
    }
    else
    {
        fprintf(stderr, "Error obtaining the group for GID %d: %s\n", gid, strerror(errno));
    }
}

void aux_listfile(bool loong, bool link, bool acc, char *fullPath, struct stat fileStat, char *fileName)
{
    struct stat isLink;
    bool isSymbolicLink = false;

    if (!(loong || link || acc))
    {
        printf("%15ld ", fileStat.st_size);
        printf(" %s\n", fileName);
        return;
    }

     if (link)
    {
        if (lstat(fullPath, &isLink) == -1)
        {
            perror("Error obtaining file link state: ");
            return;
        }
        isSymbolicLink = S_ISLNK(isLink.st_mode); // Verifica si es un enlace simbólico
    }

    if (loong)
    {
        printPermissions(isSymbolicLink ? isLink.st_mode : fileStat.st_mode);
        printf("%ld  ", fileStat.st_nlink); // número de enlaces duros
        printf("(%lu) ", fileStat.st_ino);

        struct passwd *pwd = getpwuid(fileStat.st_uid);
        if (pwd != NULL)
            printf("%s ", pwd->pw_name);
        else
        {
            perror("Error obtaining the username");
            return;
        }

        gid_t fileGid = fileStat.st_gid;
        printGroupName(fileGid);

        printf("%12ld  ", fileStat.st_size); // tamaño

        // tiempo de acceso
        struct tm *accessTime = localtime(&fileStat.st_atime);
        if (accessTime != NULL)
        {
            char formattedTime[100];
            strftime(formattedTime, sizeof(formattedTime), "%d/%m/%Y %H:%M", accessTime);
            printf("%s ", formattedTime);
        }
        else
        {
            perror("Error converting access time");
            return;
        }
    }

    if (acc && !loong)
    {
        printf("%ld ", fileStat.st_size); // tamaño

        struct tm *accessTime = localtime(&fileStat.st_atime);
        if (accessTime != NULL)
        {
            char formattedTime[100];
            strftime(formattedTime, sizeof(formattedTime), "%d/%m/%Y %H:%M", accessTime);
            printf("%s ", formattedTime);
        }
        else
        {
            perror("Error converting access time");
            return;
        }
    }

    if (link)
    {
        if (S_ISLNK(isLink.st_mode))
        {
            char linkTarget[MAX_PROMPT];
            ssize_t len = readlink(fullPath, linkTarget, sizeof(linkTarget) - 1);
            if (len == -1)
            {
                perror("Error reading the symbolic link");
                return;
            }
            linkTarget[len] = '\0';
            printf("%s -> %s", fileName, linkTarget);
        }
    }
    else
    {
        printf(" %s", fileName);
    }
    printf("\n");
}

int Cmd_listfile(char *tokens[])
{
    char *cwd = GetCwd();
    struct stat fileStat;

    if (tokens[1] == NULL)
    {
        printf("Current work directory: %s\n", cwd);
        free(cwd);
        return 0;
    }
    free(cwd);

    // seleccionar modos
    bool loong = false, link = false, acc = false;
    for (int i = 1; tokens[i] != NULL; i++)
    {
        if (tokens[i][0] == '-')
        { // si empieza por - es un modo
            if (strcmp(tokens[i], "-long") == 0)
                loong = true;
            else if (strcmp(tokens[i], "-link") == 0)
                link = true;
            else if (strcmp(tokens[i], "-acc") == 0)
                acc = true;
            else
            {
                printf("%s: Invalid option -%s\nYou can try help -%s for more informatión\n", tokens[0], tokens[i], tokens[0]);
                return -1;
            }
            // Si no son argumentos, entonces deberían de ser archivos
        }
        else
        {
            char fullPath[MAX_PROMPT];
            char *cwd = GetCwd();
            snprintf(fullPath, sizeof(fullPath), "%s/%s", cwd, tokens[i]);
            free(cwd);

            if (stat(fullPath, &fileStat) != 0)
            {
                printf("Error: file %s not found\n", tokens[i]);
                return -1;
            }
            aux_listfile(loong, link, acc, fullPath, fileStat, tokens[i]);
        }
    }
    return 0;
}

void aux_listdir(const char *dir_name, bool hid, bool loong, bool acc, bool link)
{
    DIR *dir = opendir(dir_name);
    if (dir == NULL)
    {
        printf("Error: directory %s not found\n", dir_name);
        return;
    }

    struct dirent *entry;
    struct stat fileStat, isLink;

    printf("***********%s\n", dir_name);
    while ((entry = readdir(dir)) != NULL)
    {
        // Si se permite ver archivos ocultos o el archivo no es oculto
        if (hid || entry->d_name[0] != '.')
        {
            char fullPath[MAX_PROMPT];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dir_name, entry->d_name);

            // Obtener información sobre el archivo
            if (stat(fullPath, &fileStat) != 0)
            {
                printf("Error: file %s not found\n", fullPath);
                closedir(dir);
                return;
            }

            // Manejo de enlaces simbólicos
            if (link)
            {
                if (lstat(fullPath, &isLink) == -1)
                {
                    perror("Error obtaining file link state: ");
                    closedir(dir);
                    return;
                }
                if (S_ISLNK(isLink.st_mode))
                {
                    aux_listfile(loong, link, acc, fullPath, fileStat, entry->d_name);
                }
                else
                {
                    aux_listfile(loong, false, acc, fullPath, fileStat, entry->d_name);
                }
            }
            else
            {
                aux_listfile(loong, false, acc, fullPath, fileStat, entry->d_name);
            }
        }
    }

    closedir(dir);
}

int Cmd_listdir(char *tokens[])
{

    if (tokens[1] == NULL)
    {
        Cmd_cwd();
        return 0;
    }

    // seleccionar modos
    bool loong = false, link = false, acc = false, hid = false;
    for (int i = 1; tokens[i] != NULL; i++)
    {
        if (tokens[i][0] == '-')
        { // si empieza por - es un modo
            if (strcmp(tokens[i], "-long") == 0)
                loong = true;
            else if (strcmp(tokens[i], "-link") == 0)
                link = true;
            else if (strcmp(tokens[i], "-acc") == 0)
                acc = true;
            else if (strcmp(tokens[i], "-hid") == 0)
                hid = true;
            else
            {
                printf("Invalid option for listfile, help listfile for more information\n");
                return -1;
            }
        }
        else
            aux_listdir(tokens[i], hid, loong, acc, link);
    }
    return 0;
}

void list_dir_rec(const char *dir_name, bool hid, bool loong, bool acc, bool link)
{
    DIR *dir = opendir(dir_name);
    struct dirent *entry;
    struct stat file_stat;
    char *full_path = malloc(MAX_PROMPT * sizeof(char));

    if (dir == NULL)
    {
        perror("Error opening directory");
        return;
    }

    // Leer las entradas del directorio
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            if ((!hid && entry->d_name[0] == '.'))
                continue;

            // Construimos el path completo
            char *full_path = malloc(MAX_PROMPT * sizeof(char)); // Almacenar en el heap
            snprintf(full_path, MAX_PROMPT, "%s/%s", dir_name, entry->d_name);

            // Obtener información sobre la entrada
            if (stat(full_path, &file_stat) == -1)
            {
                printf("Error al obtener información de la entrada %s: ", full_path);
                free(full_path);
                continue;
            }
            // Comprobar si es un directorio
            if (S_ISDIR(file_stat.st_mode))
            {
                aux_listdir(full_path, hid, loong, acc, link);
                // Llamada recursiva al subdirectorio
                list_dir_rec(full_path, hid, loong, acc, link);
            }
        }
    }
    free(full_path);
    closedir(dir);
}

int Cmd_reclist(char *tokens[])
{
    const int MAX_DIRECTORIES = 10;
    /*int files_hidden = 0;
    int more_info = 0;
    int dest = 0;
    int access_time = 0;*/
    const char *directories[10];
    bool loong = false, link = false, acc = false, hid = false;

    int i = 1;
    int cnt = 0;
    while (tokens[i] != NULL)
    {
        /*Comprobamos si es un parámetro o un directorio*/
        if (tokens[i][0] == '-')
        { /*Vemos que tipo de parámetro es...*/
            if (strcmp(tokens[i], "-hid") == 0)
                hid = true;
            else if (strcmp(tokens[i], "-long") == 0)
                loong = true;
            else if (strcmp(tokens[i], "-acc") == 0)
                acc = true;
            else if (strcmp(tokens[i], "-link") == 0)
                link = true;
            else
                printf("%s: Invalid option -%s\nYou can try help -%s for more informatión\n", tokens[0], tokens[i], tokens[0]);
            /*Si entra en el else es un directorio...*/
        }
        else
        {
            if (cnt == MAX_DIRECTORIES)
            {
                printf("A maximum of 10 directories can be passed\n");
                return 1;
            }
            directories[cnt++] = tokens[i];
        }
        i++;
    }

    // Si no se pasan directorios usamos el directorio actual...
    if (cnt == 0)
    {
        Cmd_cwd();
    }

    for (int j = 0; j < cnt; j++)
    {
        aux_listdir(directories[j], hid, loong, acc, link);
        list_dir_rec(directories[j], hid, loong, acc, link);
    }
    return 0;
}

void list_dir_rev(const char *dir_name, bool hid, bool loong, bool acc, bool link)
{
    DIR *dir = opendir(dir_name);
    struct dirent *entry;
    struct stat file_stat;

    if (dir == NULL)
    {
        perror("Error opening directory");
        return;
    }

    // Leer las entradas del directorio
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            if ((!hid && entry->d_name[0] == '.'))
                continue;

            // Construimos el path completo
            char *full_path = malloc(MAX_PROMPT * sizeof(char)); // Almacenar en el heap
            snprintf(full_path, MAX_PROMPT, "%s/%s", dir_name, entry->d_name);

            // Obtener información sobre la entrada
            if (stat(full_path, &file_stat) == -1)
            {
                printf("Error al obtener información de la entrada %s: ", full_path);
                free(full_path);
                continue;
            }

            // Comprobar si es un directorio
            if (S_ISDIR(file_stat.st_mode))
            {
                list_dir_rev(full_path, hid, loong, acc, link);
                aux_listdir(full_path, hid, loong, acc, link);
            }
            free(full_path);
        }
    }
    closedir(dir);
}

int Cmd_revlist(char *tokens[])
{
    const int MAX_DIRECTORIES = 10;
    /*int files_hidden = 0;
    int more_info = 0;
    int dest = 0;
    int access_time = 0;*/
    const char *directories[10];
    bool loong = false;
    bool acc = false;
    bool link = false;
    bool hid = false;

    int i = 1;
    int cnt = 0;
    while (tokens[i] != NULL)
    {
        /*Comprobamos si es un parámetro o un directorio*/
        if (tokens[i][0] == '-')
        { /*Vemos que tipo de parámetro es...*/
            if (strcmp(tokens[i], "-hid") == 0)
                hid = true;
            else if (strcmp(tokens[i], "-long") == 0)
                loong = true;
            else if (strcmp(tokens[i], "-acc") == 0)
                acc = true;
            else if (strcmp(tokens[i], "-link") == 0)
                link = true;
            else
                printf("%s: Invalid option -%s\nYou can try help -%s for more informatión\n", tokens[0], tokens[i], tokens[0]);
            /*Si entra en el else es un directorio...*/
        }
        else
        {
            if (cnt == MAX_DIRECTORIES)
            {
                printf("A maximum of 10 directories can be passed\n");
                return 1;
            }
            directories[cnt++] = tokens[i];
        }
        i++;
    }

    // Si no se pasan directorios usamos el directorio actual...
    if (cnt == 0)
    {
        Cmd_cwd();
    }

    for (int j = 0; j < cnt; j++)
    {
        list_dir_rev(directories[j], hid, loong, acc, link);
        aux_listdir(directories[j], hid, loong, acc, link);
    }
    return 0;
}

int is_directory_empty(const char *path)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("Error opening he directory");
        return 0; // no vacío en caso de error
    }

    struct dirent *entry;
    int is_empty = 1;

    // Comprobar si hay archivos o subdirectorios
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            is_empty = 0; // no está vacío si hay algo diferente de "." o ".."
            break;
        }
    }

    closedir(dir);
    return is_empty;
}

int Cmd_erase(char *tokens[])
{
    int i = 1;
    struct stat file_stat;

    while (tokens[i] != NULL)
    {
        if (stat(tokens[i], &file_stat) != 0)
        {
            printf("Error: %s not found.\n", tokens[i]);
            i++;
            continue;
        }

        if (S_ISREG(file_stat.st_mode))
        { // verificar si es un file
            if (remove(tokens[i]) == 0)
            {
                printf("File %s deleted succesfully.\n", tokens[i]);
            }
            else
            {
                perror("Error error deleting file: ");
            }
        }
        // Verificar si es un directorio
        else if (S_ISDIR(file_stat.st_mode))
        { // verificar si es un dir
            if (is_directory_empty(tokens[i]))
            {
                if (rmdir(tokens[i]) == 0)
                {
                    printf("Directory %s deleted succesfully.\n", tokens[i]);
                }
                else
                {
                    perror("Error deleting the directory: ");
                }
            }
            else
            {
                printf("Error: Directory %s is not empty.\n", tokens[i]);
            }
        }
        else
        {
            printf("Error: %s this is not a file and not a directory.\n", tokens[i]);
        }
        i++;
    }
    return 0;
}

int del_rec(const char *path)
{
    struct stat file_stat;
    DIR *dir = opendir(path);
    struct dirent *entry;

    if (dir == NULL)
    {
        perror("Error opening directory");
        return -1; // Fallo al abrir el directorio
    }

    // Leer todas las entradas del directorio
    while ((entry = readdir(dir)) != NULL)
    {
        // Ignorar los directorios especiales . y ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Construir la ruta completa
        char *full_path = malloc(strlen(path) + strlen(entry->d_name) + 2);
        sprintf(full_path, "%s/%s", path, entry->d_name);

        // Obtener información sobre la entrada
        if (stat(full_path, &file_stat) == -1)
        {
            perror("Error obtaining file status");
            free(full_path);
            continue;
        }

        // Comprobar si es un directorio
        if (S_ISDIR(file_stat.st_mode))
        {
            // Llamar recursivamente para eliminar el contenido del directorio
            del_rec(full_path); // Elimina el contenido del subdirectorio
            if (rmdir(full_path) == 0)
            {
                printf("************\n");
                printf("Directory %s deleted successfully.\n", full_path);
            }
            else
            {
                perror("Error deleting directory");
            }
        }
        else
        {
            // Si es un archivo regular, eliminarlo
            if (remove(full_path) == 0)
            {
                printf("File %s deleted successfully.\n", full_path);
            }
            else
            {
                perror("Error deleting file");
            }
        }

        free(full_path); // Liberar la memoria del path
    }

    closedir(dir); // Cerrar el directorio
    return 0;      // Éxito
}

int Cmd_delrec(char *tokens[])
{
    struct stat file_stat;

    for (int i = 1; tokens[i] != NULL; i++)
    {
        // Comprobar si el archivo o directorio existe
        if (stat(tokens[i], &file_stat) != 0)
        {
            printf("Error: %s not found.\n", tokens[i]);
            continue; // Pasar al siguiente token
        }

        // Si es un directorio, usar del_rec
        if (S_ISDIR(file_stat.st_mode))
        {
            del_rec(tokens[i]); // Llamar a la función recursiva para eliminar contenido
            if (rmdir(tokens[i]) == 0)
            {
                printf("Directory %s deleted successfully.\n", tokens[i]);
            }
            else
            {
                perror("Error deleting directory");
            }
        }
        // Si es un archivo regular
        else if (S_ISREG(file_stat.st_mode))
        {
            if (remove(tokens[i]) == 0)
            {
                printf("File %s deleted successfully.\n", tokens[i]);
            }
            else
            {
                perror("Error deleting file");
            }
        }
        else
        {
            printf("Error: %s is not a file or directory.\n", tokens[i]);
        }
    }
    return 0;
}

int Cmd_ls(char *tokens[])
{
    DIR *dir;
    struct dirent *entry;
    char *dir_name = "."; // Por defecto, listamos el directorio actual
    char *files[MAX_PROMPT];
    int count = 0;

    // Si hay un argumento, usamos ese como el directorio
    if (tokens[1] != NULL)
    {
        dir_name = tokens[1];
    }

    // Intentar abrir el directorio
    dir = opendir(dir_name);
    if (dir == NULL)
    {
        perror("Error opening directory");
        return -1; // Fallo al abrir el directorio
    }

    // Leer todas las entradas del directorio
    while ((entry = readdir(dir)) != NULL)
    {
        // Ignorar los directorios especiales . y ..
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            files[count] = strdup(entry->d_name); // Duplicar el nombre del archivo
            count++;
        }
    }

    closedir(dir); // Cerrar el directorio al finalizar

    // Imprimir los nombres de archivos en columnas
    int max_length = 0;
    for (int i = 0; i < count; i++)
    {
        if (strlen(files[i]) > max_length)
        {
            max_length = strlen(files[i]);
        }
    }

    int cols = 80 / (max_length + 2); // Determinar cuántas columnas se pueden imprimir
    for (int i = 0; i < count; i++)
    {
        printf("%-*s ", max_length + 1, files[i]); // Imprimir el nombre con un ancho fijo
        if ((i + 1) % cols == 0)
        {
            printf("\n"); // Nueva línea si alcanzamos el límite de columnas
        }
    }

    // Nueva línea final si no termina en una nueva línea
    if (count % cols != 0)
    {
        printf("\n");
    }

    // Liberar memoria
    for (int i = 0; i < count; i++)
    {
        free(files[i]); // Liberar cada nombre de archivo duplicado
    }

    return 0; // Éxito
}

char *get_formatted_cwd()
{
    // Obtener el directorio de trabajo actual
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("Error getting current working directory");
        return NULL;
    }

    // Obtener el nombre de usuario
    struct passwd *pw = getpwuid(getuid());
    if (pw == NULL)
    {
        perror("Error getting user information");
        return NULL;
    }

    // Construir la ruta de home
    char home[1024];
    snprintf(home, sizeof(home), "/home/%s", pw->pw_name);

    // Comprobar si el CWD empieza con la ruta de home
    if (strncmp(cwd, home, strlen(home)) == 0)
    {
        // Reemplazar la parte de home con "~"
        char *formatted_cwd = malloc(strlen(cwd) - strlen(home) + 2); // +2 por "~/" y el terminador
        if (formatted_cwd == NULL)
        {
            perror("Memory allocation error");
            return NULL;
        }
        snprintf(formatted_cwd, strlen(cwd) - strlen(home) + 2, "~%s", cwd + strlen(home));
        return formatted_cwd;
    }

    // Si no es la ruta de home, devolver el CWD sin cambios
    return strdup(cwd);
}

char* getCurrentDateTime() {
    time_t t = time(NULL);         // Obtiene la hora actual
    struct tm* tm_info = localtime(&t);  // Convierte a hora local
    
    // Reserva memoria para el resultado
    char* buffer = malloc(16);  // Formato "MMM DD HH:MM" requiere 16 caracteres
    if (buffer == NULL) {
        return NULL; // Manejo de errores en caso de falla de memoria
    }
    
    // Formatea la fecha y la hora
    strftime(buffer, 16, "%b %d %H:%M", tm_info);                                               
    return buffer;
}


void printAllMemList(struct node *Node, int order, int n){
    if (Node == NULL || order < 1 || n < 1)
        return;
    printAllMemList(Node->next, order - 1, --n);
    MemoryBlock *block = ((MemoryBlock*)Node->data);
    char *date = getCurrentDateTime();
    if(block->type == MALLOC_MEMORY){
        printf("%s%20p%s %12zu  %s %smalloc%s\n", GREENN, block->address, WHITE, block->size, date, ORANGE, WHITE);
    }else if(block->type == SHARED_MEMORY){
        printf("%s%20p%s %12zu  %s %sshared%s (key %d)\n",GREENN, block->address, WHITE, block->size, date, LIGHTBLUE, WHITE, block->additionalInfo.shared.key); 
    }else if(block->type == MAPPED_FILE){
        printf("%s%20p%s %12zu  %s %s%s%s (descriptor %d)\n", GREENN, block->address, WHITE, block->size, date, BLUEE, block->additionalInfo.mappedFile.name, WHITE, block->additionalInfo.mappedFile.fd);
    }
    free(date);
    
}                                                      

void printMemBlockList(struct node *Node, int order, int n){
    if (Node == NULL || order < 1 || n < 1)
        return;
    printMemBlockList(Node->next, order - 1, --n);
    MemoryBlock *block = ((MemoryBlock*)Node->data);
    char *date = getCurrentDateTime();
    if(block->type == MALLOC_MEMORY){
        printf("%s%20p%s %12zu  %s %smalloc%s\n", GREENN, block->address, WHITE, block->size, date, ORANGE, WHITE);
    }
    free(date);
}

void printMemSharedList(struct node *Node, int order, int n){
    if (Node == NULL || order < 1 || n < 1)
        return;
    printMemSharedList(Node->next, order - 1, --n);
    MemoryBlock *block = ((MemoryBlock*)Node->data);
    char *date = getCurrentDateTime();
    if(block->type == SHARED_MEMORY){
       printf("%s%20p%s %12zu  %s %sshared%s (key %d)\n",GREENN, block->address, WHITE, block->size, date, LIGHTBLUE, WHITE, block->additionalInfo.shared.key); 
    }
    free(date);
}


void printMemMapList(struct node *Node, int order, int n){
    if (Node == NULL || order < 1 || n < 1)
        return;
    printMemMapList(Node->next, order - 1, --n);
    MemoryBlock *block = ((MemoryBlock*)Node->data);
    char *date = getCurrentDateTime();
    if(block->type == MAPPED_FILE){
        printf("%s%20p%s %12zu  %s %s%s%s (descriptor %d)\n", GREENN, block->address, WHITE, block->size, date, BLUEE, block->additionalInfo.mappedFile.name, WHITE, block->additionalInfo.mappedFile.fd);
    }
    free(date);
}



int Cmd_allocate(char *tokens[]){

    int sizeList = getSize(memoryList);
    Node *firstList = first(memoryList);
    if(tokens[1] == NULL){ //Priny all lista case
        printf("\n******** Memory Blocks List allocated for the process %d *******\n\n", getpid());
        printAllMemList(firstList , sizeList, sizeList);
        printf("\n"); 
        return 0;
    }
    
    if(strcmp(tokens[1], "-malloc") == 0){
        if(tokens[2] == NULL){
            printf("******** Memory Blocks List allocated with malloc for the process %d *******\n\n", getpid());
            printMemBlockList(firstList , sizeList, sizeList);
            return 0;
        }

        if(isInteger(tokens[2])){
            size_t size = atoi(tokens[2]);
            if (size == 0) {
                printf("Invalid size: must be greater than 0\n");
                return -1;
            }

            void* memory = malloc(size);
            if (!memory) {
                printf("Failed to allocate memory\n");
                return -1;
            }

            MemoryBlock *memBlock = malloc(sizeof(MemoryBlock));
            memBlock->type = MALLOC_MEMORY;
            memBlock->address =  memory;
            memBlock->size = size;
            memBlock->allocationTime = time(NULL); //Tiempo actual
    
            insertElement(memBlock, sizeof(MemoryBlock), memoryList);
            printf("Allocated %zu bytes using malloc at %p\n", size, memory);   
        }else{
            printf("The size of malloc have to be an integer\n");
        }

    }else if(strcmp(tokens[1], "-mmap")== 0){
        if(tokens[2] == NULL || tokens[3] == NULL){
            printf("\n******** Memory Blocks List allocated with mmap for the process %d *******\n\n", getpid());
            printMemMapList(firstList , sizeList, sizeList);
            return 0;
        }

        int fd, mode = O_RDONLY;
        void *mappedMemory;
        char *fileName = tokens[2];
        char *perm;
        int protection = 0;
        struct stat st;


        if ((perm = tokens[3]) !=NULL && strlen(perm)<4) {
            if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
            if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
            if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
        }

        if (protection & PROT_WRITE) mode =O_RDWR;


        if (stat(fileName ,&st)==-1 || (fd = open(fileName , mode))==-1) return -1;

         // Verificar que el archivo no esté vacío
        if (st.st_size == 0) {
            fprintf(stderr, "Error: Cannot mmap an empty file\n");
            close(fd);
            return -1;
        }

        long pageSize = sysconf(_SC_PAGESIZE);
        if (pageSize == -1) {
            perror("Error obtaining page size");
            close(fd);
        return -1;
        }
        size_t mappedSize = (st.st_size + pageSize - 1) & ~(pageSize - 1);

        mappedMemory = mmap(NULL, st.st_size, protection, MAP_PRIVATE, fd, 0);
        if (mappedMemory == MAP_FAILED) {
            perror("Error mapping file");
            close(fd);
            return -1;
        }

        // Crear un bloque de memoria y añadirlo a la lista
        MemoryBlock *memBlock = malloc(sizeof(MemoryBlock));
        memBlock->type = MAPPED_FILE;
        memBlock->address = mappedMemory;
        memBlock->size = mappedSize;
        memBlock->allocationTime = time(NULL);
        memBlock->additionalInfo.mappedFile.name = strdup(fileName);
        memBlock->additionalInfo.mappedFile.fd = fd;

        if (insertElement(memBlock, sizeof(MemoryBlock), memoryList) == -1) {
            perror("Error inserting memory block into list");
            free(memBlock); // Liberar memoria si no se pudo insertar
            return -1;
        }

                // crear archivo
        OpenFile *file = malloc(sizeof(OpenFile));
        file->name = strdup(fileName);
        file->fd = fd;
        file->mode = mode;
        file->offset = 0;
        file->type = MAPPED;

        insertElement(file, sizeof(OpenFile), openList);

        printf("Mapped file %s of size %zu at %p\n", fileName, (size_t)st.st_size, mappedMemory);

    }else if(strcmp(tokens[1], "-createshared") == 0){  
        key_t key;
        size_t size;
        void *shmAdr;
        int flags = 0777, id, aux;
        struct shmid_ds s;

        if (tokens[2] == NULL || tokens[3] == NULL) {
            printf("\n******** Memory Blocks List shared for the process %d *******\n\n", getpid());
            printMemSharedList(firstList , sizeList, sizeList);
	        return 0;
        }
  
        key =(key_t)  strtoul(tokens[2],NULL,10);
        size =(size_t) strtoul(tokens[3],NULL,10);

        if (size == 0) {
	        printf ("It can't create blocks of 0 bytes\n");
	        return -1;
        }
        

        flags = flags | IPC_CREAT | IPC_EXCL;

        if(key == IPC_PRIVATE){
            errno = EINVAL; 
            return -1;
        }

        if ((id = shmget(key , size, flags))==-1){
            if (errno == EEXIST) printf("Error: Shared memory with key %d already exists.\n", key);
            else if(errno == ENOMEM) printf("There aren't enough avalaible memory in the system\n");
            else if(errno == ENOSPC) printf("Error: The maximum number of segments allowed has been reached.\n");
            return -1;
        }
        if ((shmAdr = shmat(id,NULL,0))==(void*) -1){
            aux = errno;
            shmctl(id,IPC_RMID,NULL);
            errno = aux;
            return -1;
        }
        shmctl (id,IPC_STAT, &s);

        // Crear un bloque de memoria y añadirlo a la lista
        MemoryBlock *memBlock = malloc(sizeof(MemoryBlock));
        memBlock->address = shmAdr;
        memBlock->size = size; 
        memBlock->allocationTime = time(NULL);
        memBlock->type = SHARED_MEMORY;
        memBlock->additionalInfo.shared.key = key;

        if (insertElement(memBlock, sizeof(MemoryBlock), memoryList) == -1) {
            perror("Error inserting memory block into list");
            free(memBlock);
            return -1;
        }
        printf("Attached to shared memory block with key %d at %p\n", key, shmAdr);

    } else if(strcmp(tokens[1], "-shared") == 0){
        
        if(tokens[2] == NULL){
            printf("******** Memory Blocks List shared for the process %d *******\n\n", getpid());
            printMemSharedList(firstList , sizeList, sizeList);
            return 0;       
        }
        
        int key = atoi(tokens[2]);

        int shmId = shmget(key, 0, 0666);
        if (shmId == -1) {
            perror("Error accessing shared memory");
            return -1;
        }

        void *shmAddr = shmat(shmId, NULL, 0);
        if (shmAddr == (void *)-1) {
            perror("Error attaching shared memory");
            return -1;
        }

        struct shmid_ds shmInfo;
        if (shmctl(shmId, IPC_STAT, &shmInfo) == -1) {
            perror("Error retrieving shared memory info");
            shmdt(shmAddr);
            return -1;
        }

            MemoryBlock *memBlock = malloc(sizeof(MemoryBlock));
            memBlock->address = shmAddr;
            memBlock->size = shmInfo.shm_segsz;
            memBlock->allocationTime = time(NULL);
            memBlock->type = SHARED_MEMORY;
            memBlock->additionalInfo.shared.key = key;

        if (insertElement(memBlock, sizeof(MemoryBlock), memoryList) == -1) {
            perror("Error inserting memory block into list");
            free(memBlock);
            return -1;
        }
        printf("Attached shared memory with key=%d, address=%p, size=%zu bytes\n",
                   key, shmAddr, shmInfo.shm_segsz);
    
    }else{
        printf("The flag %s doesn't exist,  try \"help allocate\" for more information\n", tokens[1]);
        return -1;
    }

    return 0;
}


int Cmd_deallocate(char *tokens[]) {
    
    if (!tokens[1]) {
        printf("Usage: deallocate <option>\n");
        return -1;
    }

    if (strcmp(tokens[1], "-malloc") == 0) {
        if (tokens[2] && isInteger(tokens[2])) {
            size_t size = atoi(tokens[2]);

            if (!removeElement( &size , MEM,memoryList)) { // Usa tu función exist
                perror("Error removing block from list");
                return -1;
            }
            printf("Deallocated malloc block of size %zu\n", size);
        } else {
            printf("Usage: deallocate -malloc <size>\n");
        }
    } else if (strcmp(tokens[1], "-mmap") == 0) {
        if (tokens[2]) {
            
            if (!removeElement(tokens[2],MAP, memoryList)) {
                perror("Error removing block from list");
                return -1;
            }
            printf("Unmapped file %s from memory\n", tokens[2]);
        } else {
            printf("Usage: deallocate -mmap <file>\n");
        }
    } else if (strcmp(tokens[1], "-shared") == 0) {
        
        if (tokens[2] && isInteger(tokens[2])) {
            int key = atoi(tokens[2]);
            if (!removeElement(&key, SHARED,memoryList)) {
                perror("Error removing block from list");
                return -1;
            }
            printf("Detached shared memory block with key %d\n", key);
        } else {
            printf("Usage: deallocate -shared <key>\n");
        }

    } else if (strcmp(tokens[1], "-delkey") == 0) {
        if (tokens[2] && isInteger(tokens[2])) {
            int key = atoi(tokens[2]);
            int shmid = shmget(key, 0, 0666);

            if (shmid == -1) {
                perror("Error getting shared memory ID");
                return -1;
            }

            if (shmctl(shmid, IPC_RMID, NULL) == -1) {
                perror("Error removing shared memory key");
                return -1;
            }

            /*if(!removeElement(&key , SHARED, memoryList)){
                printf("Error removing the element of the list\n");
                return -1;
            }*/

            printf("Removed shared memory key %d from the system\n", key);
        } else {
            fprintf(stderr, "Usage: deallocate -delkey <key>\n");
        }

    } else  {
        void *addr = (void *)strtoull(tokens[1], NULL, 16);

        if (!removeElement(addr, ADDR,memoryList)) {
            perror("Error removing block from list");
            return -1;
        }
        printf("Deallocated memory block at address %p\n", addr);
    }
    
    return 0;
}

int Cmd_memfill(char *tokens[]){
    if (!tokens[1] || !tokens[2] || !tokens[3]) {
        fprintf(stderr, "Usage: memfill <addr> <cont> <ch>\n");
        return -1;
    }

    void *addr = (void *)strtoul(tokens[1], NULL, 0);

    size_t cont = (size_t)atoi(tokens[2]);
    unsigned char ch = (unsigned char)tokens[3][0];

    size_t max_size = getBlockSize(addr);

    if (addr == NULL) {
        fprintf(stderr, "Invalid address.\n");
        return -1;
    }

    if (cont == 0) {
        fprintf(stderr, "Byte count must be greater than 0.\n");
        return -1;
    }

    if(max_size != -1 && cont > max_size){
        fprintf(stderr, "The size of the memfill can't more than the size of the block,\n the max size for this block is %ld\n", max_size);
        return -1;
    }

    // Llenar la memoria con el carácter especificado
    memset(addr, ch, cont);

    printf("Filled memory at address %p with %zu bytes of character '%c'.\n", addr, cont, ch);
    return 0;
}

int Cmd_memdump(char *tokens[]) {
    if (!tokens[1] || !tokens[2]) {
        printf("Usage: memdump <address> <count>\n");
        return -1;
    }

    void *address = (void *)strtol(tokens[1], NULL, 16);
    int count = atoi(tokens[2]);

    if (count <= 0) {
        printf("Error: count must be greater than 0.\n");
        return -1;
    }

    unsigned char *ptr = (unsigned char *)address;

    printf("Memory dump at address %p:\n", address);
    for (int i = 0; i < count; i++) {
        // Imprimir hexadecimal
        printf("%02x ", ptr[i]);

        // Insertar salto de línea cada 16 bytes
        if ((i + 1) % 16 == 0) {
            printf(" | ");
            // Imprimir caracteres imprimibles
            for (int j = i - 15; j <= i; j++) {
                if (isprint(ptr[j])) {
                    printf("%c", ptr[j]);
                } else {
                    printf(".");
                }
            }
            printf("\n");
        }
    }

    // Finalizar con los caracteres imprimibles restantes si no termina en múltiplo de 16
    int remaining = count % 16;
    if (remaining != 0) {
        // Rellenar espacio para alinear caracteres imprimibles
        for (int i = 0; i < (16 - remaining) * 3; i++) {
            printf(" ");
        }
        printf(" | ");
        for (int i = count - remaining; i < count; i++) {
            if (isprint(ptr[i])) {
                printf("%c", ptr[i]);
            } else {
                printf(".");
            }
        }
        printf("\n");
    }

    return 0;
}


void recurse(int n, const int length, char* staticArray){
    char stackArray[length];
    printf("parametro: %d(%p) array %p, arr estatico %p\n", n, &n, stackArray, staticArray);
    if(n<1){
        return;
    }
    recurse(n - 1, length, staticArray);
}

int Cmd_recurse(char **tokens){
    if(tokens[1]==NULL){
        return 0;
    }else{
        if(isInteger(tokens[1])){
            int n = atoi(tokens[1]);
            if(n < 0){
                fprintf(stderr, "recurse: the depth must be positive\n");
            }
            const int length = 2048;
            char staticArray[length];
            recurse(n,length,staticArray);
            return 0;
        }
    }
    return -1;
}

void memoryFuncs(){
    printf("Funciones de programa %p, %p, %p\n", Cmd_date, Cmd_authors, Cmd_authors);
    printf("Funciones de libreria %p, %p, %p\n", malloc,atoi,isalpha);
}

int ex1, ex2, ex3;

void memoryVars(){
    int a,b,c;
    static int s1;
    static int s2;
    static int s3;
    static int sn1 = 100;
    static int sn2 = 200;
    static int sn3 = 300;
    printf("Variables locales %p, %p, %p\n", &a, &b, &c);
    printf("Variables globales %p, %p, %p\n", &commandHistory,&openList,&memoryList);
    printf("Variables globales(N.I) %p %p %p\n", &ex1, &ex2, &ex3);
    printf("Variables estaticas %p %p %p\n", &sn1, &sn2, &sn3);
    printf("Variables estaticas(N.I) %p %p %p\n", &s1, &s2, &s3);
}

void memoryPmap(){
    FILE *aux;
    char command[MAX_PROMPT/2];
    char path[MAX_PROMPT];
    pid_t pid;

    pid = getpid();
    snprintf(command,sizeof(command),"pmap -x %d", pid);

    aux = popen(command, "r"); 

    if (aux == NULL) {
        perror("popen");
        return;
    }

    while (fgets(path, sizeof(path)-1, aux) != NULL) {
        printf("%s", path);
    }

    pclose(aux);
}

int Cmd_memory(char **tokens){
    int sizeList = getSize(memoryList);
    Node *firstList = first(memoryList);
    if(tokens[1]==NULL){
        memoryVars();
        memoryFuncs();
        printf("\n******** Memory Blocks List allocated for the process %d *******\n\n", getpid());
    } else if(strcmp(tokens[1], "-funcs")==0) memoryFuncs();
    else if(strcmp(tokens[1], "-vars")==0) memoryVars();
    else if(strcmp(tokens[1], "-all")==0){
        memoryFuncs();
        memoryVars();
        printf("\n******** Memory Blocks List allocated for the process %d *******\n\n", getpid());
        printAllMemList(firstList , sizeList, sizeList);
    } else if(strcmp(tokens[1], "-blocks")==0) {
        printf("\n******** Memory Blocks List allocated for the process %d *******\n\n", getpid());
        printAllMemList(firstList , sizeList, sizeList);
    }else if(strcmp(tokens[1], "-pmap")== 0) memoryPmap();
    else printf("%s: Invalid option -%s\nYou can try help -%s for more informatión\n", tokens[0], tokens[1], tokens[0]);
    return 0;
}


int Cmd_writefile(char *tokens[]) {
    char *filename = tokens[1];
    void *addr =  (void *)strtol(tokens[2], NULL, 16);
    size_t cont = atoi(tokens[3]);
    
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    size_t bytesWritten = fwrite(addr, 1, cont, file);
    if (bytesWritten < cont) {
        perror("Error writing to file");
        fclose(file);
        return -1;
    }

    fclose(file);
    printf("Wrote %zu bytes from address %p to file '%s'\n", bytesWritten, addr, filename);
    return 0;
}


ssize_t readfile(char *file, void *addr, size_t cont) {
    struct stat s;
    ssize_t n;
    int fd, aux;

    if (stat(file, &s) == -1 || (fd = open(file, O_RDONLY)) == -1)
        return -1;

    if (cont == -1) // Leer todo el archivo
        cont = s.st_size;

    if ((n = read(fd, addr, cont)) == -1) {
        aux = errno;
        close(fd);
        errno = aux;
        return -1;
    }

    close(fd);
    return n;
}

int Cmd_readfile(char *tokens[]) {
    void *addr;
    size_t cont = -1;
    ssize_t n;

    if (!tokens[1] || !tokens[2]) {
        printf("Usage: readfile <file> <address> [<bytes>]\n");
        return -1;
    }


    addr = (void *)strtol(tokens[2], NULL, 16);
    if (tokens[3])
        cont = (size_t)atoll(tokens[3]);

    n = readfile(tokens[1], addr, cont);
    if (n == -1){
        perror("Error reading file");
        return -1;
    }
    printf("Read %lld bytes from %s into %p\n", (long long)n, tokens[1], addr);
    return 0;
}

int Cmd_read(char *tokens[]) {
    if(!isInteger(tokens[1])){
        fprintf(stderr, "The fd have to be a integer\n");
        return -1;
    }
    int fd = atoi(tokens[1]);
    void *addr =  (void *)strtol(tokens[2], NULL, 16);
    size_t cont = atoi(tokens[3]);
    
    ssize_t bytesRead = read(fd, addr, cont);
    if (bytesRead == -1) {
        perror("Error reading from file descriptor");
        return -1;
    }

    printf("Read %zd bytes from descriptor %d to address %p\n", bytesRead, fd, addr);
    return 0;
}



int Cmd_write(char *tokens[]) {

    if(!isInteger(tokens[1])){
        fprintf(stderr, "The fd have to be a integer\n");
        return -1;
    }
    int fd = atoi(tokens[1]);
    void *addr =  (void *)strtol(tokens[2], NULL, 16);
    size_t cont = atoi(tokens[3]);
    

    ssize_t bytesWritten = write(fd, addr, cont);
    if (bytesWritten == -1) {
        perror("Error writing to file descriptor");
        return -1;
    }

    printf("Wrote %zd bytes from address %p to descriptor %d\n", bytesWritten, addr, fd);
    return 0;
}


int Cmd_getuid(){
    
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();

    struct passwd *real_pwd = getpwuid(real_uid);
    struct passwd *effective_pwd = getpwuid(effective_uid);

    printf("Real user: %d, (%s)\n", real_uid, real_pwd ? real_pwd->pw_name : "unknow");
    printf("Effective user: %d, (%s)\n", geteuid(), effective_pwd ? effective_pwd->pw_name : "unknow");

    return 0;
}

int Cmd_setuid(char *tokens[]){

    int login_mode = 0;
    uid_t new_uid;
    struct passwd *pwd;

    if (tokens[1] == NULL) {
        Cmd_getuid();
        return 0;
    }

    // Verificar si el modo login está activado
    if ((strcmp(tokens[1], "-l")== 0)){
          login_mode = 1;
        if (tokens[2] == NULL) {
            printf("Error: the username after '-l'\n");
            return -11;
        }

        // Buscar información del usuario por nombre
        pwd = getpwnam(tokens[2]);
        if (!pwd) {
            printf("Error: user '%s' not found\n", tokens[2]);
            return 1;
        }
        new_uid = pwd->pw_uid;
    } else if(isInteger(tokens[1])){
        new_uid = (uid_t) atoi(tokens[1]);
    }else{
        printf("The id have to be an integer\n");
        return -1;
    }

    // Obtener información del usuario
    pwd = getpwuid(new_uid);
    if (!pwd) {
        perror("Error obtaining user information\n");
        return -1;
    }

    if (seteuid(new_uid) != 0) {
        perror("Error cahnging the effective ID\n");
        return -1;
    }

    printf("Effective Id have been changed: %d, (%s)\n", new_uid, pwd->pw_name);

    if (login_mode) {
        // Simular login: establecer variables de entorno
        setenv("USER", pwd->pw_name, 1);
        setenv("HOME", pwd->pw_dir, 1);
        setenv("SHELL", pwd->pw_shell, 1);

        printf("Login mode activated: configured as %s\n", pwd->pw_name);
    }

    return 0;
}


void printEnv(){
    printf("Showing all environment variables:\n");
    char **env_ptr = globalEnvp;
    uintptr_t address; // Para almacenar la dirección de la variable de entorno.

    // Iteramos a través de las variables de entorno.
    for (int i = 0; env_ptr[i] != NULL; i++) {
        address = (uintptr_t) &globalEnvp[i];
        // Imprimimos la dirección y el índice, y el valor de la variable de entorno.
        printf("%p->main arg3[%d]=%p) %s\n", (void *) address, i, (void *) env_ptr[i], env_ptr[i]);
    }
}

int Cmd_showvar(char *tokens[]) {
    if (tokens[1] == NULL) { // If no variables are provided, display all environment variables
        printEnv();
        return 0;
    }

     char *flag = tokens[1]; // The flag should be the second token

    if (strcmp(flag, "-a") == 0) {
        if (tokens[2] == NULL || tokens[3] == NULL) {
            printf("Usage: changevar -a var val\n");
            return -1;
        }

        //implementar aqui -a

    } else if (strcmp(flag, "-e") == 0) {
        // Modify an existing variable
        if (tokens[2] == NULL || tokens[3] == NULL) {
            printf("Usage: changevar -e var val\n");
            return -1;
        }
        if (setenv(tokens[2], tokens[3], 1) == 0) {
            printf("Variable '%s' updated with value '%s'.\n", tokens[2], tokens[3]);
        } else {
            perror("Error modifying variable with setenv");
            return -1;
        }
    } else if (strcmp(flag, "-p") == 0) {
        // Add or modify a variable using putenv
        if (tokens[2] == NULL || tokens[3] == NULL) {
            printf("Usage: changevar -p var val\n");
            return -1;
        }
        char varAssignment[256];
        snprintf(varAssignment, sizeof(varAssignment), "%s=%s", tokens[2], tokens[3]);
        if (putenv(varAssignment) == 0) {
            printf("Variable '%s' set with value '%s'.\n", tokens[2], tokens[3]);
        } else {
            perror("Error setting variable with putenv");
            return -1;
        }
    } else {
        printf("Invalid flag. Usage: changevar [-a|-e|-p] var val\n");
        return -1;
    }


    return 0;
}

int Cmd_subsvar(char *tokens[]) {

    if (tokens[1] == NULL) { // Si no se pasan argumentos o flags
        printf("Usage: subsvar [-a|-e] v1 v2 val\n");
        return -1;
    }

    char *flag = tokens[1]; // La flag debe ser el segundo token

    if (tokens[2] == NULL || tokens[3] == NULL || tokens[4] == NULL) {
        printf("Usage: subsvar [-a|-e] v1 v2 val\n");
        return -1;
    }

    if (strcmp(flag, "-a") == 0) {
        // La flag -a permite reemplazar v1 con v2 y establecer un nuevo valor
        if (getenv(tokens[2]) == NULL) { // Si v1 no existe
            printf("Variable '%s' does not exist. Use -e to modify an existing variable.\n", tokens[2]);
            return -1;
        }

        // Crear la nueva variable v2 con el valor dado
        char varAssignment[256];
        snprintf(varAssignment, sizeof(varAssignment), "%s=%s", tokens[3], tokens[4]);
        if (putenv(varAssignment) == 0) {
            printf("Variable '%s' created with value '%s'.\n", tokens[3], tokens[4]);
        } else {
            perror("Error creating variable with putenv");
            return -1;
        }
    } else if (strcmp(flag, "-e") == 0) {
        // La flag -e permite modificar v1 si existe, reemplazándola con v2 y el valor dado
        if (getenv(tokens[2]) == NULL) { // Si v1 no existe
            printf("Variable '%s' not found. Use -a to create a new variable.\n", tokens[2]);
            return -1;
        }

        // Modificar v1 con el nuevo valor y reemplazar con v2
        char varAssignment[256];
        snprintf(varAssignment, sizeof(varAssignment), "%s=%s", tokens[3], tokens[4]);
        if (putenv(varAssignment) == 0) {
            printf("Variable '%s' replaced with '%s' and value '%s'.\n", tokens[2], tokens[3], tokens[4]);
        } else {
            perror("Error modifying variable with putenv");
            return -1;
        }
    } else {
        printf("Invalid flag. Usage: subsvar [-a|-e] v1 v2 val\n");
        return -1;
    }

    return 0;
}

int Cmd_environ(char *tokens[]) {
    if (tokens[1] == NULL) { // Si no se pasa ninguna flag
       printEnv();
    }

    // Verifica si la flag es válida
    if (strcmp(tokens[1], "-environ") == 0) {
        printEnv();
    } else if (strcmp(tokens[1], "-addr") == 0) {
         for (char **env = globalEnvp; *env != NULL; env++) {
        printf("environ:   %p (almacenado en %p)\n", (void*)*env, (void*)env);
    }
    } else {
        printf("Invalid flag. Usage: environ [-environ|-addr]\n");
        return -1;
    }

    return 0;
}

int Cmd_help(char **tokens)
{
    if (tokens[1] == NULL)
    {
        // Muestra ayuda general sobre los comandos
        printf("Available commands:\n");
        printf("  authors [options] - Prints the names and logins of the program authors.\n");
        printf("  pid - Prints the pid of the process executing the shell.\n");
        printf("  ppid - Prints the pid of the shell's parent process.\n");
        printf("  cd [dir] - Changes the current working directory. Without arguments prints the current directory.\n");
        printf("  date [-t|-d] - Prints the current date and time in specific formats.\n");
        printf("  historic [N|-N] - Shows the history of commands executed by the shell.\n");
        printf("  open [file] mode - Adds a file to the list of open files.\n");
        printf("  close [df] - Closes the specified file descriptor.\n");
        printf("  dup [df] - Duplicates the specified file descriptor.\n");
        printf("  infosys - Prints information about the machine running the shell.\n");
        printf("  makefile [name] - Creates a file with the specified name.\n");
        printf("  makedir [name] - Creates a directory with the specified name.\n");
        printf("  listfile [-long][-link][-acc] name1 name2 .. - Lists files with options.\n");
        printf("  cwd - Shows the current working directory of the shell.\n");
        printf("  listdir [-hid][-long][-link][-acc] n1 n2 .. - Lists directory contents with options.\n");
        printf("  reclist [-hid][-long][-link][-acc] n1 n2 .. - Recursively lists directory contents (subdirs after).\n");
        printf("  revlist [-hid][-long][-link][-acc] n1 n2 .. - Recursively lists directory contents (subdirs before).\n");
        printf("  erase [name1 name2 ..] - Deletes empty files or directories.\n");
        printf("  delrec [name1 name2 ..] - Recursively deletes files or directories.\n");
        printf("  exit, quit, bye - Ends the shell.\n");
    }
    else
    {
        // Ayuda específica para un comando
        if (strcmp(tokens[1], "authors") == 0)
        {
            printf("authors - Prints the names and logins of the program authors.\n");
            printf("  authors -l - Prints only the logins.\n");
            printf("  authors -n - Prints only the names.\n");
        }
        else if (strcmp(tokens[1], "pid") == 0)
        {
            printf("pid - Prints the pid of the process executing the shell.\n");
        }
        else if (strcmp(tokens[1], "ppid") == 0)
        {
            printf("ppid - Prints the pid of the shell's parent process.\n");
        }
        else if (strcmp(tokens[1], "cd") == 0)
        {
            printf("cd [dir] - Changes the current working directory to dir.\n");
            printf("  Without arguments, prints the current working directory.\n");
        }
        else if (strcmp(tokens[1], "date") == 0)
        {
            printf("date [-t|-d] - Prints the current date and time.\n");
            printf("  date -d - Prints the current date in the format DD/MM/YYYY.\n");
            printf("  date -t - Prints the current time in the format hh:mm:ss.\n");
        }
        else if (strcmp(tokens[1], "historic") == 0)
        {
            printf("historic [N|-N] - Shows the history of commands executed by this shell.\n");
            printf("  historic - Prints all commands.\n");
            printf("  historic N - Repeats command number N from the history list.\n");
            printf("  historic -N - Prints only the last N commands.\n");
        }
        else if (strcmp(tokens[1], "open") == 0)
        {
            printf("open [file] mode - Adds a file to the list of open files.\n");
            printf("  Modes: cr, ap, ex, ro, rw, wo, tr.\n");
            printf("  Open without arguments lists the open files.\n");
        }
        else if (strcmp(tokens[1], "close") == 0)
        {
            printf("close [df] - Closes the specified file descriptor df.\n");
        }
        else if (strcmp(tokens[1], "dup") == 0)
        {
            printf("dup [df] - Duplicates the specified file descriptor df.\n");
        }
        else if (strcmp(tokens[1], "infosys") == 0)
        {
            printf("infosys - Prints information about the machine running the shell.\n");
        }
        else if (strcmp(tokens[1], "makefile") == 0)
        {
            printf("makefile [name] - Creates a file with the specified name.\n");
        }
        else if (strcmp(tokens[1], "makedir") == 0)
        {
            printf("makedir [name] - Creates a directory with the specified name.\n");
        }
        else if (strcmp(tokens[1], "listfile") == 0)
        {
            printf("listfile [-long][-link][-acc] name1 name2 .. - Lists files with options.\n");
            printf("  -long: Detailed list.\n");
            printf("  -link: Shows the path if it is a symbolic link.\n");
            printf("  -acc: Access time.\n");
        }
        else if (strcmp(tokens[1], "cwd") == 0)
        {
            printf("cwd - Shows the current working directory of the shell.\n");
        }
        else if (strcmp(tokens[1], "listdir") == 0)
        {
            printf("listdir [-hid][-long][-link][-acc] n1 n2 .. - Lists directory contents with options.\n");
            printf("  -long: Detailed list.\n");
            printf("  -hid: Includes hidden files.\n");
            printf("  -link: Shows the path if it is a symbolic link.\n");
            printf("  -acc: Access time.\n");
        }
        else if (strcmp(tokens[1], "reclist") == 0)
        {
            printf("reclist [-hid][-long][-link][-acc] n1 n2 .. - Recursively lists directory contents (subdirs after).\n");
            printf("  -long: Detailed list.\n");
            printf("  -hid: Includes hidden files.\n");
            printf("  -link: Shows the path if it is a symbolic link.\n");
            printf("  -acc: Access time.\n");
        }
        else if (strcmp(tokens[1], "revlist") == 0)
        {
            printf("revlist [-hid][-long][-link][-acc] n1 n2 .. - Recursively lists directory contents (subdirs before).\n");
            printf("  -long: Detailed list.\n");
            printf("  -hid: Includes hidden files.\n");
            printf("  -link: Shows the path if it is a symbolic link.\n");
            printf("  -acc: Access time.\n");
        }
        else if (strcmp(tokens[1], "erase") == 0)
        {
            printf("erase [name1 name2 ..] - Deletes empty files or directories.\n");
        }
        else if (strcmp(tokens[1], "delrec") == 0)
        {
            printf("delrec [name1 name2 ..] - Recursively deletes files or directories.\n");
        }
        else if (strcmp(tokens[1], "exit") == 0 || strcmp(tokens[1], "quit") == 0 || strcmp(tokens[1], "bye") == 0)
        {
            printf("exit, quit, bye - Ends the shell.\n");
        }
        else
        {
            printf("Unknown command: %s\n", tokens[1]);
            return 1;
        }
    }
    return 0;
}

int Cmd_exit()
{
    printf("Exiting shell...\n");
    return 2;
}

Command commands[] = {
    {"authors", Cmd_authors},
    {"pid", Cmd_pid},
    {"ppid", Cmd_ppid},
    {"cd", Cmd_cd},
    {"date", Cmd_date},
    {"historic", Cmd_historic},
    {"open", Cmd_open},
    {"memory", Cmd_memory},
    {"close", Cmd_close},
    {"dup", Cmd_dup},
    {"infosys", Cmd_infosys},
    {"help", Cmd_help},
    {"makefile", Cmd_makefile},
    {"makedir", Cmd_makedir},
    {"listfile", Cmd_listfile},
    {"cwd", Cmd_cwd},
    {"recurse", Cmd_recurse},
    {"listdir", Cmd_listdir},
    {"reclist", Cmd_reclist},
    {"revlist", Cmd_revlist},
    {"erase", Cmd_erase},
    {"delrec", Cmd_delrec},
    {"allocate", Cmd_allocate},
    {"deallocate", Cmd_deallocate},
    {"memfill", Cmd_memfill},
    {"memdump", Cmd_memdump},
    {"read", Cmd_read},
    {"readfile", Cmd_readfile},
    {"writefile", Cmd_writefile},
    {"write", Cmd_write},
    {"getuid", Cmd_getuid},
    {"setuid", Cmd_setuid},
    {"showvar", Cmd_showvar},
    {"subsvar", Cmd_subsvar},
    {"environ", Cmd_environ},
    {"quit", Cmd_exit},
    {"exit", Cmd_exit},
    {"bye", Cmd_exit},
    {"ls", Cmd_ls},
    {NULL, NULL},
};
