#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct cursor {
  int x;
  int y;
} user_cursor;

void display(int height, int width, int **cell) {
  system("clear");
  printf ("H J K L: Move cursor | ");
  printf ("SPACE: Set cell | ");
  printf ("Enter: Start game / Next generation | ");
  printf ("Q: Game exit\r\n\r\n");
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if ((x == user_cursor.x) && (y == user_cursor.y)) {
        // printf("[+]");
        printf("🔴");
        continue;
      }
      if (cell[y][x]) {
        // printf("[@]");
        printf("⬛");
      } else {
        // printf("[ ]");
        printf("⬜");
      }
    }
    printf("\r\n");
  }
}

int get_cell_state(int h, int w, int y, int x, int **cell) {
  if ( x < 0 || x >= w || y < 0 || y >= h )
    return 0;
  return cell[y][x];
}

int get_alive_count(int h, int w, int y, int x, int **cell) {
  return get_cell_state(h, w, y - 1, x - 1, cell)
  + get_cell_state(h, w, y - 1, x, cell)
  + get_cell_state(h, w, y - 1, x + 1, cell)
  + get_cell_state(h, w, y, x - 1, cell)
  + get_cell_state(h, w, y, x + 1, cell)
  + get_cell_state(h, w, y + 1, x - 1, cell)
  + get_cell_state(h, w, y + 1, x, cell)
  + get_cell_state(h, w, y + 1, x + 1, cell);
}

int get_next_state(int h, int w, int y, int x, int **cell) {
  int alive_count = get_alive_count(h, w, y, x, cell);
  if ( cell[y][x] == 1 ) {
    if ( alive_count == 2 || alive_count == 3 )
      return 1;
    if ( alive_count <= 1)
      return 0;
    if ( alive_count >= 4)
      return 0;
  }
  if (cell[y][x] == 0 ) {
    if (alive_count == 3)
      return 1;
  }
  return cell[y][x];
}

void init_array(int val, int height, int width, int **cell) {
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      cell[y][x] = val;
}

void copy_array(int height, int width, int **src, int **dst) {
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      dst[y][x] = src[y][x];
}

int is_cursor_inside(int height, int width, int x, int y) {
  if ( x < 0 || x >= width || y < 0 || y >= height )
    return 0;
  else
    return 1;
}

int main(int argc, const char *argv[]) {
  int **cell;
  int **next_cell;
  int height = 10;
  int width = 10;

  if (argc == 3 ) {
    height = atoi(argv[1]);
    if (height <= 0) {
      fprintf(stderr, "height should be > 0: %s\n", argv[1]);
      exit(EXIT_FAILURE);
    }

    width = atoi(argv[2]);
    if (width <= 0) {
      fprintf(stderr, "width should be > 0: %s\n", argv[2]);
      exit(EXIT_FAILURE);
    }
  }

  cell = (int **)malloc(height * sizeof(int *));
  next_cell = (int **)malloc(height * sizeof(int *));
  for (int y = 0; y < height; y++) {
    cell[y] = (int *)malloc(width * sizeof(int));
    next_cell[y] = (int *)malloc(width * sizeof(int));
  }

  init_array (0, height, width, cell);
  copy_array (height, width, cell, next_cell);
  system ("/bin/stty raw");
  while (1) {
    display(height, width, cell);
    switch (getchar()) {
      case 'h': if (is_cursor_inside(height, width, user_cursor.x-1, user_cursor.y)) user_cursor.x--; break;
      case 'j': if (is_cursor_inside(height, width, user_cursor.x, user_cursor.y+1)) user_cursor.y++; break;
      case 'k': if (is_cursor_inside(height, width, user_cursor.x, user_cursor.y-1)) user_cursor.y--; break;
      case 'l': if (is_cursor_inside(height, width, user_cursor.x+1, user_cursor.y)) user_cursor.x++; break;
      case ' ': cell[user_cursor.y][user_cursor.x] = 1 - cell[user_cursor.y][user_cursor.x]; break;
      case 'q':
                system ("/bin/stty cooked");
                return 0;
                break;
      // Enter key
      case 13: goto START_GAME; break;
    }
  }
  START_GAME:
  user_cursor.x = -1;
  user_cursor.y = -1;
  while (1) {
    display(height, width, cell);
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        next_cell[y][x] = get_next_state(height, width, y, x, cell);
      }
    }
    copy_array (height, width, next_cell, cell);
    // sleep(1);
    switch (getchar()) {
      case 'q':
                system ("/bin/stty cooked");
                return 0;
                break;
      // Enter key
      case 13: ; break;
    }
  }
  return 0;
}
