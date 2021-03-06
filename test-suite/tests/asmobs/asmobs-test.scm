#!./asmobs -s
!#

(define (test x v)
  (if v
      (if (not (equal? (eval x (current-module)) v))
	  (error "Wrong return value" x))
      (catch #t
	     (lambda ()
	       (begin (eval x (current-module))
		      (throw 'should-be-error)))
	     (lambda (key . args)
	       (if (eq? key 'should-be-error)
		   (error "Should be error" x))))))

(define asmob000 (make-asmob000))
(test '(asmob000) '())
(test '(asmob000 1) #f)
(test '(asmob000 1 2) #f)
(test '(asmob000 1 2 3) #f)

(define asmob100 (make-asmob100))
(test '(asmob100) #f)
(test '(asmob100 1) '(1))
(test '(asmob100 1 2) #f)
(test '(asmob100 1 2 3) #f)

(define asmob010 (make-asmob010))
(test '(asmob010) '(#f))
(test '(asmob010 1) '(1))
(test '(asmob010 1 2) #f)
(test '(asmob010 1 2 3) #f)

(define asmob001 (make-asmob001))
(test '(asmob001) '(()))
(test '(asmob001 1) '((1)))
(test '(asmob001 1 2) '((1 2)))
(test '(asmob001 1 2 3) '((1 2 3)))

(define asmob200 (make-asmob200))
(test '(asmob200) #f)
(test '(asmob200 1) #f)
(test '(asmob200 1 2) '(1 2))
(test '(asmob200 1 2 3) #f)

(define asmob110 (make-asmob110))
(test '(asmob110) #f)
(test '(asmob110 1) '(1 #f))
(test '(asmob110 1 2) '(1 2))
(test '(asmob110 1 2 3) #f)

(define asmob020 (make-asmob020))
(test '(asmob020) '(#f #f))
(test '(asmob020 1) '(1 #f))
(test '(asmob020 1 2) '(1 2))
(test '(asmob020 1 2 3) #f)

(define asmob101 (make-asmob101))
(test '(asmob101) #f)
(test '(asmob101 1) '(1 ()))
(test '(asmob101 1 2) '(1 (2)))
(test '(asmob101 1 2 3) '(1 (2 3)))

(define asmob011 (make-asmob011))
(test '(asmob011) '(#f ()))
(test '(asmob011 1) '(1 ()))
(test '(asmob011 1 2) '(1 (2)))
(test '(asmob011 1 2 3) '(1 (2 3)))

(define asmob300 (make-asmob300))
(test '(asmob300) #f)
(test '(asmob300 1) #f)
(test '(asmob300 1 2) #f)
(test '(asmob300 1 2 3) '(1 2 3))

(define asmob210 (make-asmob210))
(test '(asmob210) #f)
(test '(asmob210 1) #f)
(test '(asmob210 1 2) '(1 2 #f))
(test '(asmob210 1 2 3) '(1 2 3))

(define asmob120 (make-asmob120))
(test '(asmob120) #f)
(test '(asmob120 1) '(1 #f #f))
(test '(asmob120 1 2) '(1 2 #f))
(test '(asmob120 1 2 3) '(1 2 3))

(define asmob030 (make-asmob030))
(test '(asmob030) '(#f #f #f))
(test '(asmob030 1) '(1 #f #f))
(test '(asmob030 1 2) '(1 2 #f))
(test '(asmob030 1 2 3) '(1 2 3))

(define asmob201 (make-asmob201))
(test '(asmob201) #f)
(test '(asmob201 1) #f)
(test '(asmob201 1 2) '(1 2 ()))
(test '(asmob201 1 2 3) '(1 2 (3)))

(define asmob021 (make-asmob021))
(test '(asmob021) '(#f #f ()))
(test '(asmob021 1) '(1 #f ()))
(test '(asmob021 1 2) '(1 2 ()))
(test '(asmob021 1 2 3) '(1 2 (3)))

(define asmob111 (make-asmob111))
(test '(asmob111) #f)
(test '(asmob111 1) '(1 #f ()))
(test '(asmob111 1 2) '(1 2 ()))
(test '(asmob111 1 2 3) '(1 2 (3)))

(display "OK\n")
