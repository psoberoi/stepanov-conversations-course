; Bigrams
; By: Justin Meiners (2018)
; written for Chicken Scheme
; This uses linked list lisp style rather than the contiguous array fotran style, 
; so it should be much slower.

(use srfi-13)
(use extras)

; lexographical comparison
(define (bigram<? a b)
  (if (string=? (car a) (car b))
      (string<? (cdr a) (cdr b))
      (string<? (car a) (car b))))

(define (bigram=? a b)
  (and (string=? (car a) (car b))
       (string=? (cdr a) (cdr b))))

(define (next-token port)
  (read-token char-whitespace? port)
  (read-token (lambda (c) (not (char-whitespace? c))) port))

(define (next-bigram first port)
  (let ((token (next-token port)))
    (if (= (string-length token) 0)
        '()
        (cons first token)))) 

(define (make-bigram-index port)
  (define (add-next buffer previous-bigram)
    (let ((result (next-bigram (cdr previous-bigram) port)))
      (if (null? result)
          buffer
          (begin 
            (add-next (cons result buffer) result)))))
  (add-next '() (next-bigram '() port)))

; returns the count and the remaining list
; (count, remaining)
(define (count-if items pred)
  (define (next n ls)
    (if (or (null? ls) (not (pred (car ls))))
        (cons n ls) 
        (next (+ n 1) (cdr ls))))
  (next 0 items))

(define (merge-duplicates bigrams)
  (define (merge-item ls buffer)
    (let* ((bigram (car ls))
           (pair (count-if ls (lambda (other) (bigram=? bigram other))))
           (new-buffer (cons (cons bigram (car pair)) buffer)))
      (if (null? (cdr pair))
          new-buffer
          (merge-item (cdr pair) new-buffer))))
  (if (pair? bigrams)
      (merge-item bigrams '())
      '()))

(define (cost-pair<? a b) (> (cdr a) (cdr b)))

(define (display-results results max-count)
  (if (and (pair? results) (> max-count 0))
      (begin
        (display (car (car results)))
        (display " ")
        (display (cdr (car results)))
        (newline)
        (display-results (cdr results) (- max-count 1)))
      (display "done")))

(if (< (length (command-line-arguments)) 1)
    (begin
      (display "one path argument expected")
      (newline))
    (let* ((file-port (open-input-file (car (command-line-arguments))))
           (index (make-bigram-index file-port))
           (sorted-index (sort! index bigram<?))
           (merged (merge-duplicates sorted-index))
           (sorted-merged (sort! merged cost-pair<?)))
      (display-results sorted-merged 250)))

