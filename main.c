#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define SCORE_FILE "scores.txt"
#define MAX_NAME 50
#define PEGS 4
#define COLORS 6
#define MAX_POSS (1296) /* COLORS^PEGS = 6^4 = 1296 */
#define MAX 100
#define N 6
#define EPS 1e-6

/* ---------- COUNT-TO-REACH (Le compte est bon) ---------- */
typedef struct {
    double value; // numeric value
    char expr[200]; // expression that generated value
} Node;

/* make target & found global so recursive solve can access them */
static int target_global = 0;
static int found_global = 0;

void solve(Node nums[], int size) {
    /* If already found an exact solution, short-circuit */
    if (found_global) return;

    /* base case */
    if (size == 1) {
        if (fabs(nums[0].value - target_global) < EPS) {
            printf("Solution : %s = %.2f\n", nums[0].expr, nums[0].value);
            found_global = 1;
        }
        return;
    }

    /* try all pairs */
    for (int i = 0; i < size && !found_global; i++) {
        for (int j = i + 1; j < size && !found_global; j++) {
            Node a = nums[i];
            Node b = nums[j];
            Node next[N];
            int idx = 0;
            for (int k = 0; k < size; k++)
                if (k != i && k != j) next[idx++] = nums[k];

            /* try 6 operations */
            Node ops[6];
            ops[0].value = a.value + b.value;
            sprintf(ops[0].expr, "(%s + %s)", a.expr, b.expr);

            ops[1].value = a.value - b.value;
            sprintf(ops[1].expr, "(%s - %s)", a.expr, b.expr);

            ops[2].value = b.value - a.value;
            sprintf(ops[2].expr, "(%s - %s)", b.expr, a.expr);

            ops[3].value = a.value * b.value;
            sprintf(ops[3].expr, "(%s * %s)", a.expr, b.expr);

            if (fabs(b.value) > EPS) {
                ops[4].value = a.value / b.value;
                sprintf(ops[4].expr, "(%s / %s)", a.expr, b.expr);
            } else ops[4].value = 1e18;

            if (fabs(a.value) > EPS) {
                ops[5].value = b.value / a.value;
                sprintf(ops[5].expr, "(%s / %s)", b.expr, a.expr);
            } else ops[5].value = 1e18;

            for (int o = 0; o < 6 && !found_global; o++) {
                next[idx] = ops[o];
                solve(next, size - 1);
            }
        }
    }
}

void defi_compte() {
    Node nums[N];
    printf("Entrez %d nombres (ex: 25 100 3 6 8 4) :\n", N);
    for (int i = 0; i < N; i++) {
        double x;
        if (scanf("%lf", &x) != 1) {
            printf("Lecture invalide.\n");
            while (getchar() != '\n'); // clear buffer
            return;
        }
        nums[i].value = x;
        /* format without decimal when integer-like */
        if (fabs(x - round(x)) < EPS)
            sprintf(nums[i].expr, "%.0f", x);
        else
            sprintf(nums[i].expr, "%.2f", x);
    }

    printf("Objectif (entier) : ");
    if (scanf("%d", &target_global) != 1) {
        printf("Lecture invalide.\n");
        while (getchar() != '\n'); // clear buffer
        return;
    }

    found_global = 0;
    solve(nums, N);
    if (!found_global)
        printf("Aucune solution exacte trouvee.\n");

    // Clear input buffer
    while (getchar() != '\n');
}

/* ---------- MASTERMIND ---------- */
typedef struct {
    int p[PEGS];
} Code;

void print_code(const Code *c) {
    for (int i = 0; i < PEGS; ++i) {
        printf("%d", c->p[i]);
        if (i < PEGS - 1) printf(" ");
    }
}

Code random_code() {
    Code c;
    for (int i = 0; i < PEGS; ++i)
        c.p[i] = (rand() % COLORS) + 1;
    return c;
}

int read_code(Code *c) {
    for (int i = 0; i < PEGS; ++i) {
        if (scanf("%d", &c->p[i]) != 1) return 0;
        if (c->p[i] < 1 || c->p[i] > COLORS) return 0;
    }
    return 1;
}

void feedback(const Code *secret, const Code *guess, int *blacks, int *whites) {
    int sec_count[COLORS+1] = {0};
    int g_count[COLORS+1] = {0};
    *blacks = 0;
    *whites = 0;

    for (int i = 0; i < PEGS; ++i) {
        if (secret->p[i] == guess->p[i])
            (*blacks)++;
        else {
            sec_count[ secret->p[i] ]++;
            g_count[ guess->p[i] ]++;
        }
    }

    for (int color = 1; color <= COLORS; ++color)
        *whites += (sec_count[color] < g_count[color]) ? sec_count[color] : g_count[color];
}

int generate_all(Code all[]) {
    int idx = 0;
    for (int a = 1; a <= COLORS; ++a)
        for (int b = 1; b <= COLORS; ++b)
            for (int c = 1; c <= COLORS; ++c)
                for (int d = 1; d <= COLORS; ++d) {
                    all[idx].p[0] = a;
                    all[idx].p[1] = b;
                    all[idx].p[2] = c;
                    all[idx].p[3] = d;
                    idx++;
                }
    return idx;
}

int filter(const Code in[], int in_n, const Code *guess, int obs_blacks, int obs_whites, Code out[]) {
    int out_n = 0;
    for (int i = 0; i < in_n; ++i) {
        int b, w;
        feedback(&in[i], guess, &b, &w);
        if (b == obs_blacks && w == obs_whites)
            out[out_n++] = in[i];
    }
    return out_n;
}

void human_play_mode() {
    printf("Mode: vous devinez le code.\nCode secret aleatoire genere.\nEntrez vos propositions: 1 2 3 4\n");
    Code secret = random_code();
    int attempts = 0;

    while (1) {
        attempts++;
        printf("Proposition #%d: ", attempts);
        Code guess;
        if (!read_code(&guess)) {
            printf("Lecture invalide. Entrez %d nombres entre 1 et %d.\n", PEGS, COLORS);
            while (getchar() != '\n');
            attempts--;
            continue;
        }

        int b, w;
        feedback(&secret, &guess, &b, &w);
        printf("Reponse: %d blacks, %d whites\n", b, w);

        if (b == PEGS) {
            printf("Bravo ! Vous avez trouve en %d essais.\n", attempts);
            break;
        }
    }
}

void computer_solve_mode() {
    printf("Mode: l'ordinateur resout votre code.\nEntrez le code secret (ex: 1 2 3 4): ");
    Code secret;
    if (!read_code(&secret)) {
        printf("Lecture invalide. Fin.\n");
        while (getchar() != '\n');
        return;
    }

    Code all[MAX_POSS];
    int total = generate_all(all);
    Code possible[MAX_POSS];
    memcpy(possible, all, sizeof(all));
    int poss_n = total;
    int attempts = 0;

    while (1) {
        attempts++;
        Code guess = possible[0];
        int b, w;
        feedback(&secret, &guess, &b, &w);
        printf("Essai #%d: ", attempts);
        print_code(&guess);
        printf(" -> %d blacks, %d whites\n", b, w);

        if (b == PEGS) {
            printf("L'ordinateur a trouve le code en %d essais.\n", attempts);
            break;
        }

        Code next_possible[MAX_POSS];
        int next_n = filter(possible, poss_n, &guess, b, w, next_possible);
        poss_n = next_n;

        if (poss_n <= 0) {
            printf("Aucune possibilite restante : inconsistance detectee. Fin.\n");
            break;
        }
        memcpy(possible, next_possible, poss_n * sizeof(Code));

        if (attempts > 50) {
            printf("Trop d'essais (>50). Abandon.\n");
            break;
        }
    }
}

int defi_mastermind() {
    srand((unsigned)time(NULL));
    printf("=== Mastermind algorithmique (C) ===\n");
    printf("Parametres: %d positions, couleurs 1..%d\n", PEGS, COLORS);
    printf("Choisissez le mode:\n 1 - Vous devinez\n 2 - L'ordinateur resout\nEntrez 1 ou 2: ");

    int mode = 0;
    if (scanf("%d", &mode) != 1) {
        printf("Entree invalide. Fin.\n");
        while (getchar() != '\n');
        return 1;
    }

    while (getchar() != '\n'); // clear buffer

    if (mode == 1)
        human_play_mode();
    else if (mode == 2)
        computer_solve_mode();
    else
        printf("Mode inconnu. Fin.\n");

    return 0;
}

/* ---------- LABYRINTHE (BFS) ---------- */
typedef struct {
    int x, y;
} Point;

int dx[4] = {-1, 1, 0, 0};
int dy[4] = {0, 0, 1, -1};
char dirChar[4] = {'N','S','E','O'};

int BFS(char grid[MAX][MAX], int R, int C, Point start, Point goal,
        Point parent[MAX][MAX], int moveDir[MAX][MAX]) {
    int visited[MAX][MAX] = {0};
    Point queue[MAX*MAX];
    int front = 0, back = 0;

    queue[back++] = start;
    visited[start.x][start.y] = 1;

    while (front < back) {
        Point cur = queue[front++];
        if (cur.x == goal.x && cur.y == goal.y) return 1;

        for (int d = 0; d < 4; d++) {
            int nx = cur.x + dx[d];
            int ny = cur.y + dy[d];
            if (nx >= 0 && nx < R && ny >= 0 && ny < C &&
                !visited[nx][ny] && grid[nx][ny] != '#') {
                visited[nx][ny] = 1;
                parent[nx][ny] = cur;
                moveDir[nx][ny] = d;
                queue[back++] = (Point){nx, ny};
            }
        }
    }
    return 0;
}

int reconstruct_path(Point parent[MAX][MAX], int moveDir[MAX][MAX],
                     Point start, Point goal, char path[]) {
    char temp[10000];
    int len = 0;
    Point cur = goal;

    while (!(cur.x == start.x && cur.y == start.y)) {
        int d = moveDir[cur.x][cur.y];
        temp[len++] = dirChar[d];
        cur = parent[cur.x][cur.y];
    }

    for (int i = 0; i < len; i++)
        path[i] = temp[len - 1 - i];
    path[len] = '\0';
    return len;
}

int defi_laby() {
    int R, C;
    char grid[MAX][MAX];
    Point start = {-1,-1}, goal = {-1,-1};

    printf("Entrez R C (rows & columns): ");
    if (scanf("%d %d", &R, &C) != 2) {
        printf("Lecture invalide.\n");
        while (getchar() != '\n');
        return 0;
    }

    printf("Entrez la grille (R lignes, sans espaces, '#' murs, 'S' start, 'G' goal):\n");
    while (getchar() != '\n'); // clear buffer

    for (int i = 0; i < R; i++) {
        if (scanf("%s", grid[i]) != 1) {
            printf("Lecture invalide.\n");
            return 0;
        }
        for (int j = 0; j < C; j++) {
            if (grid[i][j] == 'S') start = (Point){i,j};
            if (grid[i][j] == 'G') goal = (Point){i,j};
        }
    }

    if (start.x == -1 || goal.x == -1) {
        printf("S ou G manquant.\n");
        return 0;
    }

    Point parent[MAX][MAX];
    int moveDir[MAX][MAX];

    if (!BFS(grid, R, C, start, goal, parent, moveDir)) {
        printf("Aucun chemin trouve.\n");
        return 0;
    }

    char path[10000];
    reconstruct_path(parent, moveDir, start, goal, path);
    printf("\nChemin trouve : %s\n", path);

    return 0;
}

/* ---------- TRI (selection sort instrumenté) ---------- */
long long nbComparaisons = 0;
long long nbEchanges = 0;
long long limiteOps = 0;

void echanger(int *a, int *b) {
    if (limiteOps && (nbComparaisons + nbEchanges >= limiteOps)) return;
    int temp = *a;
    *a = *b;
    *b = temp;
    nbEchanges++;
}

int comparer_val(int a, int b) {
    if (limiteOps && (nbComparaisons + nbEchanges >= limiteOps)) return 0;
    nbComparaisons++;
    return (a < b); /* true if a < b -> useful to find minimum for ascending order */
}

void selectionSort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) {
            if (comparer_val(arr[j], arr[minIndex]))
                minIndex = j;
        }
        if (minIndex != i)
            echanger(&arr[minIndex], &arr[i]);
        if (limiteOps && (nbComparaisons + nbEchanges >= limiteOps)) return;
    }
}

void afficher(int arr[], int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int defi_tri() {
    int n;
    printf("Taille du tableau : ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Lecture invalide.\n");
        while (getchar() != '\n');
        return 0;
    }

    int *tab = malloc(n * sizeof(int));
    if (!tab) {
        printf("Memoire insuffisante.\n");
        return 0;
    }

    srand((unsigned)time(NULL));
    printf("Tableau initial : ");
    for (int i = 0; i < n; i++) {
        tab[i] = rand() % 100;
        printf("%d ", tab[i]);
    }
    printf("\n");

    printf("Entrer une limite d'operations (0 = pas de limite) : ");
    if (scanf("%lld", &limiteOps) != 1) limiteOps = 0;

    nbComparaisons = nbEchanges = 0;
    selectionSort(tab, n);

    printf("\nTableau apres tri : ");
    afficher(tab, n);
    printf("\n=== STATISTIQUES ===\n");
    printf("Comparaisons : %lld\n", nbComparaisons);
    printf("Echanges : %lld\n", nbEchanges);
    printf("Total : %lld\n", nbComparaisons + nbEchanges);

    if (limiteOps && (nbComparaisons + nbEchanges >= limiteOps))
        printf("\n Limite d'operations atteinte, tri interrompu.\n");

    free(tab);
    while (getchar() != '\n'); // clear buffer
    return 0;
}

/* ---------- SCORES: keep simple ---------- */
typedef struct {
    char nom[MAX_NAME];
    int score;
} ScoreEntry;

void ajouterScore(const char *nom, int score) {
    FILE *f = fopen(SCORE_FILE, "a");
    if (!f) {
        printf("Erreur : impossible d'ouvrir %s\n", SCORE_FILE);
        return;
    }
    fprintf(f, "%s %d\n", nom, score);
    fclose(f);
}

int comparerScores(const void *a, const void *b) {
    const ScoreEntry *sa = a;
    const ScoreEntry *sb = b;
    return sb->score - sa->score; // descending
}

void afficherClassement() {
    FILE *f = fopen(SCORE_FILE, "r");
    if (!f) {
        printf("Aucun score enregistre pour le moment.\n");
        return;
    }

    ScoreEntry scores[1000];
    int count = 0;
    while (count < 1000 && fscanf(f, "%s %d", scores[count].nom, &scores[count].score) == 2)
        count++;
    fclose(f);

    if (count == 0) {
        printf("Aucun score trouve.\n");
        return;
    }

    qsort(scores, count, sizeof(ScoreEntry), comparerScores);
    printf("\n===== CLASSEMENT DES MEILLEURS PROGRAMMEURS =====\n");
    for (int i = 0; i < count; i++)
        printf("%2d. %-20s %5d pts\n", i + 1, scores[i].nom, scores[i].score);
}

/* ---------- MENU ---------- */
void menu() {
    printf("\n===== ARENA DE DEFIS ALGORITHMIQUES =====\n");
    printf("1. Defi - Le Compte est Bon\n");
    printf("2. Mastermind Algorithmique\n");
    printf("3. Course de Robots (Labyrinthe)\n");
    printf("4. Tri Ultime\n");
    printf("5. Voir le classement\n");
    printf("0. Quitter\n");
}

int main() {
    FILE *fp = fopen("ascii_art.txt", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            printf("%s", line);
        }
        fclose(fp);
    }

    char nom[50];
    printf("\nNom du joueur : ");
    if (scanf("%49s", nom) != 1) {
        printf("Erreur de lecture du nom.\n");
        return 1;
    }
    while (getchar() != '\n'); // Clear the input buffer

    int choix;
    char buffer[100];
    int valide;

    do {
        menu();

        // Boucle pour forcer une entrée valide
        do {
            printf("Votre choix (0 a 5) : ");

            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                printf("Erreur de lecture.\n");
                return 1;
            }

            // Essayer de convertir l'entrée en nombre
            valide = 1; // supposer que c'est valide
            if (sscanf(buffer, "%d", &choix) != 1) {
                printf("Entree invalide ! Veuillez entrer un nombre.\n");
                valide = 0;
                continue;
            }

            // Vérifier si le choix est dans la plage valide
            if (choix < 0 || choix > 5) {
                printf("Choix invalide ! Veuillez entrer un nombre entre 0 et 5.\n");
                valide = 0;
            }
        } while (!valide); // Répéter tant que l'entrée n'est pas valide

        switch (choix) {
            case 1:
                defi_compte();
                break;
            case 2:
                defi_mastermind();
                break;
            case 3:
                defi_laby();
                break;
            case 4:
                defi_tri();
                break;
            case 5:
                afficherClassement();
                break;
            case 0:
                printf("Au revoir %s !\n", nom);
                break;
        }

        if (choix != 0) {
            int continuer;
            printf("\n");

            // Demander si l'utilisateur veut continuer
            do {
                printf("Si tu veux rejouer (taper 1)\n");
                printf("Si tu veux quitter (taper 0)\n");
                printf("Ton choix : ");

                if (scanf("%d", &continuer) != 1) {
                    printf("Lecture invalide.\n");
                    while (getchar() != '\n');
                    continuer = -1; // forcer une nouvelle itération
                } else {
                    while (getchar() != '\n'); // Clear buffer

                    if (continuer != 0 && continuer != 1) {
                        printf("Choix invalide ! Tapez 0 ou 1.\n");
                    }
                }
            } while (continuer != 0 && continuer != 1);

            if (continuer == 0) {
                printf("Au revoir %s !\n", nom);
                choix = 0; // Pour sortir de la boucle principale
            }
        }

    } while (choix != 0);

    return 0;
}
