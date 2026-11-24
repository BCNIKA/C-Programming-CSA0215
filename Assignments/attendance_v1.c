/*
 * Student Attendance and Performance Analysis System - Version 1
 * C Programming | Assignment 2 | CO2
 * Student : Yashwanth Bewan A (Reg. No. 1921260214)
 *
 * Build : gcc -Wall -Wextra -Wpedantic -Wshadow -std=c11 -o attendance_v1 attendance_v1.c
 * Note  : compile with -DECHO_INPUT to echo redirected input (used for test transcripts)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS    60      /* array capacity                          */
#define MAX_ID          99999   /* largest valid student ID                */
#define MAX_MARK        100     /* internal marks are out of 100           */
#define MIN_ATTENDANCE  75.0f   /* attendance eligibility threshold (%)    */
#define PASS_MARK       40      /* minimum internal mark for the exam      */
#define SECOND_MARK     50      /* lower limit of Second Class             */
#define FIRST_MARK      70      /* lower limit of First Class              */
#define DIST_MARK       85      /* lower limit of Distinction              */
#define LINE_LEN        64

/* ---------------- input helpers: every value is validated ---------------- */
static int read_line(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL)
        return 0;                                  /* end of input            */
    if (strchr(buf, '\n') == NULL) {               /* over-long line: discard */
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
#ifdef ECHO_INPUT
    printf("%s\n", buf);
#endif
    return 1;
}

static void input_ended(void)
{
    printf("\nInput ended unexpectedly. Program stopped.\n");
    exit(EXIT_FAILURE);
}

static int read_int(const char *prompt, int lo, int hi)
{
    char line[LINE_LEN];
    char *end;
    long v = 0;
    int ok = 0;

    while (!ok) {                                  /* retry until valid       */
        printf("%s", prompt);
        if (!read_line(line, (int)sizeof line))
            input_ended();
        v = strtol(line, &end, 10);
        if (end == line || *end != '\0')
            printf("  Invalid input: enter a whole number.\n");
        else if (v < lo || v > hi)
            printf("  Out of range: allowed %d to %d.\n", lo, hi);
        else
            ok = 1;
    }
    return (int)v;
}

static double read_double(const char *prompt, double lo, double hi)
{
    char line[LINE_LEN];
    char *end;
    double v = 0.0;
    int ok = 0;

    while (!ok) {
        printf("%s", prompt);
        if (!read_line(line, (int)sizeof line))
            input_ended();
        v = strtod(line, &end);
        if (end == line || *end != '\0')
            printf("  Invalid input: enter a number.\n");
        else if (!(v >= lo && v <= hi))            /* also rejects NaN        */
            printf("  Out of range: allowed %.0f to %.0f.\n", lo, hi);
        else
            ok = 1;
    }
    return v;
}

static int read_choice(const char *prompt)         /* -1 means "not a number" */
{
    char line[LINE_LEN];
    char *end;
    long v;

    printf("%s", prompt);
    if (!read_line(line, (int)sizeof line))
        input_ended();
    v = strtol(line, &end, 10);
    if (end == line || *end != '\0' || v < 0 || v > 9999)
        return -1;
    return (int)v;
}

/* ---------------- decision-making ---------------- */
static int is_eligible(float attendance)
{
    return attendance >= MIN_ATTENDANCE;           /* exactly 75.0 is eligible */
}

static const char *category(int mark)              /* else-if ladder           */
{
    if (mark >= DIST_MARK)
        return "Distinction";
    else if (mark >= FIRST_MARK)
        return "First Class";
    else if (mark >= SECOND_MARK)
        return "Second Class";
    else if (mark >= PASS_MARK)
        return "Pass";
    else
        return "Fail";
}

static void exam_status(float attendance, int mark) /* nested if               */
{
    if (attendance >= MIN_ATTENDANCE) {
        if (mark >= PASS_MARK)
            printf("ELIGIBLE for the examination.\n");
        else
            printf("NOT ELIGIBLE - low mark (below %d).\n", PASS_MARK);
    } else {
        printf("NOT ELIGIBLE - attendance shortage (below %.0f%%).\n", MIN_ATTENDANCE);
    }
}

/* ---------------- array processing ---------------- */
static void read_records(int ids[], float att[], int marks[], int n)
{
    int i, j, id, duplicate;

    for (i = 0; i < n; i++) {
        printf("--- Student %d of %d ---\n", i + 1, n);
        do {
            duplicate = 0;
            id = read_int("  ID (1-99999): ", 1, MAX_ID);
            for (j = 0; j < i; j++) {              /* only populated elements */
                if (ids[j] == id) {
                    duplicate = 1;
                    printf("  Duplicate ID. Enter a unique ID.\n");
                    break;
                }
            }
        } while (duplicate);
        ids[i]   = id;
        att[i]   = (float)read_double("  Attendance % (0-100): ", 0.0, 100.0);
        marks[i] = read_int("  Internal mark (0-100): ", 0, MAX_MARK);
    }
}

static void display_records(const int ids[], const float att[], const int marks[], int n)
{
    int i;

    printf("\n%-4s %-6s %-10s %-6s %-9s %s\n",
           "No.", "ID", "Attend.%", "Mark", "Eligible", "Category");
    for (i = 0; i < n; i++)
        printf("%-4d %-6d %-10.1f %-6d %-9s %s\n", i + 1, ids[i], att[i], marks[i],
               is_eligible(att[i]) ? "Yes" : "No", category(marks[i]));
}

static int linear_search(const int ids[], int n, int key)
{
    int i;

    for (i = 0; i < n; i++) {
        if (ids[i] == key)
            return i;                              /* index of first match    */
    }
    return -1;                                     /* not found               */
}

static void display_statistics(const int ids[], const float att[], const int marks[], int n)
{
    int i, total = 0, hi = 0, lo = 0;
    int eligible = 0, above = 0, below = 0, equal = 0;
    double average;

    for (i = 0; i < n; i++) {                      /* pass 1: total, max, min */
        total += marks[i];
        if (marks[i] > marks[hi])
            hi = i;
        if (marks[i] < marks[lo])
            lo = i;
        if (is_eligible(att[i]))
            eligible++;
    }
    average = (double)total / n;                   /* no integer truncation   */

    for (i = 0; i < n; i++) {                      /* pass 2: vs. average     */
        if (marks[i] > average)
            above++;
        else if (marks[i] < average)
            below++;
        else
            equal++;
    }

    printf("\n===== CLASS STATISTICS (N = %d) =====\n", n);
    printf("Total marks         : %d\n", total);
    printf("Average mark        : %.2f\n", average);
    printf("Highest mark        : %d (ID %d)\n", marks[hi], ids[hi]);
    printf("Lowest mark         : %d (ID %d)\n", marks[lo], ids[lo]);
    printf("Eligible students   : %d (attendance >= %.1f%%)\n", eligible, MIN_ATTENDANCE);
    printf("Ineligible students : %d\n", n - eligible);
    printf("Above average       : %d\n", above);
    printf("Below average       : %d\n", below);
    printf("Equal to average    : %d\n", equal);
}

static void show_eligibility(int id, float a, int m)
{
    printf("Student %d: attendance %.1f%%, internal mark %d\n", id, a, m);
    printf("  Attendance eligibility : %s\n", is_eligible(a) ? "ELIGIBLE" : "NOT ELIGIBLE");
    printf("  Performance category   : %s\n", category(m));
    printf("  Exam status            : ");
    exam_status(a, m);
}

/* ---------------- menu ---------------- */
static void run_menu(const int ids[], const float att[], const int marks[], int n)
{
    int choice, key, pos;

    do {                                           /* menu shows at least once */
        printf("\n===== MENU =====\n");
        printf("1. Display all records\n");
        printf("2. Check one student's eligibility\n");
        printf("3. Display class statistics\n");
        printf("4. Search student by ID\n");
        printf("5. Exit\n");
        choice = read_choice("Enter choice (1-5): ");

        switch (choice) {
        case 1:
            display_records(ids, att, marks, n);
            break;
        case 2:
            key = read_int("Enter student ID: ", 1, MAX_ID);
            pos = linear_search(ids, n, key);
            if (pos >= 0)
                show_eligibility(ids[pos], att[pos], marks[pos]);
            else
                printf("Student ID %d not found.\n", key);
            break;
        case 3:
            display_statistics(ids, att, marks, n);
            break;
        case 4:
            key = read_int("Enter student ID to search: ", 1, MAX_ID);
            pos = linear_search(ids, n, key);
            if (pos >= 0)
                printf("Found: ID %d at position %d (array index %d), attendance %.1f%%, mark %d.\n",
                       key, pos + 1, pos, att[pos], marks[pos]);
            else
                printf("Student ID %d not found.\n", key);
            break;
        case 5:
            printf("Exiting the program. Goodbye.\n");
            break;
        default:
            printf("Invalid choice. Please enter a number from 1 to 5.\n");
        }
    } while (choice != 5);                         /* ends only on Exit        */
}

int main(void)
{
    int   ids[MAX_STUDENTS]   = {0};               /* initialised arrays       */
    float att[MAX_STUDENTS]   = {0.0f};
    int   marks[MAX_STUDENTS] = {0};
    int   n;

    printf("STUDENT ATTENDANCE AND PERFORMANCE ANALYSIS SYSTEM\n");
    n = read_int("Enter number of students (1-60): ", 1, MAX_STUDENTS);
    read_records(ids, att, marks, n);
    run_menu(ids, att, marks, n);
    return 0;
}
