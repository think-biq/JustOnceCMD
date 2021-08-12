

#define GENERIC_MAX(x, y) ((x) > (y) ? (x) : (y))
#define GENERIC_MIN(x, y) ((x) < (y) ? (x) : (y))

#define ENSURE_int(i)   _Generic((i), int:   (i))
#define ENSURE_float(f) _Generic((f), float: (f))

#define MAX(type, x, y) \
  (type)GENERIC_MAX(ENSURE_##type(x), ENSURE_##type(y))
#define MIN(type, x, y) \
  (type)GENERIC_MIN(ENSURE_##type(x), ENSURE_##type(y))