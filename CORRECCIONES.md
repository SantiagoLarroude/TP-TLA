# Modificar
- [x] Cambiar `eq`, `neq`, etc por `==`, `!=`, etc
- [ ] No abreviar tanto los nombres de no-terminales y/o funciones, variables, etc
- [x] Hacer un dispatch sobre la producción, en lugar de en la función
- [ ] Sacar macros para el log, reemplazar por funciones
- [x] El condicional debe evaluar true/false únicamente
- [x] Modificar `bool_expr -> expr AND expr` y similares para evitar hacer la verificación en el backend
    - [ ] Verificar si un `identifier` vale True/False o una función devuelve True/False.


# Agregar
- [x] Nombre de proyecto
- [x] Instrucciones de compilación (en README) 
    - [ ] Probar en Pampero
- [x] Dependencias y versiones (en README)
- [x] Ejemplos de código archivos separados (fuera del README, aunque puede haber un ejemplo corto dentro del mismo).
- [x] Script que pruebe los ejemplos de código
- [ ] Precedencia con paréntesis en operaciones aritméticas (gramática) sin conflictos S/R y R/R.

# Mantener
- [x] IF con ELSE obligatorio
- [x] Operadores distintos para strings (en lugar de hacer sobrecarga).
- [x] Código sin conflictos S/R y R/R.

# Verificar pre-entrega
- [ ] Licencia con nombre de los integrantes

# Analizar
- [ ] Uso de EOL para cerrar las instrucciones (en lugar de `;`). Por ahora hay un `.\n`
- [x] For each con aspecto funcional (iteración interna) en lugar de imperativo (iteración externa)


