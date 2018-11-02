;; -*- mode: emacs-lisp  -*-
;;
;; k0-mode.el -- k0 mode
;;
;; $OpenXM$

;; This program is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; Todo: some codes for windows from asir-mode.el have not been deleted.
;;

;; 1. Install 
;;
;;
;; **(for unix) Copy this file to your emacs site-lisp folder and 
;;              write the following configuration to your .emacs file.
;;
;; (setq auto-mode-alist (cons '("\\.k$" . k0-mode) auto-mode-alist))
;; (autoload 'k0-mode "k0-mode" "K0 mode" t)
;;
;; Please run byte-compile for speed up.
;;
;; 2. Use
;;
;; If you open K0 source file (*.rr) by emacs, then k0-mode starts up automatically.
;; The following key binding can be used:
;; C-c s     K0 starts up in another window.
;; C-c t     K0 terminates.
;; C-c a     Abort current calculation.
;; C-c l     The current buffer is loaded to K0 as a file.
;; C-c r     Selected region is loaded to K0 as a file.
;; C-c p     Selected region is pasted to K0.

(require 'shell)
(require 'cl)

;;;; K0GUI for Windows   These from asir-mode.el have not been deleted.
(defvar k0-exec-path '("~/Desktop/k0/bin" "c:/Program Files/k0/bin" "c:/Program Files (x64)/k0/bin" "c:/k0/bin")
  "Default search path for k0 binary in Windows")

(defun k0-effective-exec-path ()
  "Search path for command"
  (let* ((dir (getenv "K0_ROOTDIR"))
         (path (append k0-exec-path exec-path)))
    (if dir (cons (concat dir "/bin") path) path)))

(defun k0-executable-find (command)
  "Search for command"
  (let* ((exec-path (k0-effective-exec-path)))
    (executable-find command)))

;;;; K0 for UNIX
(defvar k0-cmd-buffer-name "*k0-cmd*")

(defun k0-cmd-load (filename)
  "Send `load' command to running k0 process"
  (if (eq system-type 'windows-nt)
      (let ((exec-path (k0-effective-exec-path)))
        (start-process "k0-proc-cmdk0" nil "cmdk0" filename))
    (save-excursion
      (if (get-buffer k0-cmd-buffer-name)
          (progn
            (set-buffer k0-cmd-buffer-name)
            (goto-char (point-max))
            (insert (format "load[\"%s\"];;" filename))
            (comint-send-input))))))

(defun k0-start ()
  "Start k0 process"
  (interactive)
  (if (eq system-type 'windows-nt)
    ;; for Windows
      (let ((exec-path (k0-effective-exec-path)))
        (start-process "k0-proc-k0gui" nil "k0gui"))
    ;; for UNIX
    (save-excursion
      (if (not (get-buffer k0-cmd-buffer-name))
          (let ((current-frame (selected-frame)))
            (if window-system 
                (progn 
                  (select-frame (make-frame))
                  (shell (get-buffer-create k0-cmd-buffer-name)) ;; Switch to new buffer automatically
                  (delete-other-windows))
              (if (>= emacs-major-version 24)
                  (progn 
                    (split-window)
                    (other-window -1)))
              (shell (get-buffer-create k0-cmd-buffer-name)))
            (sleep-for 1)
            (goto-char (point-max))
            (insert "openxm k0")
            (comint-send-input)
            (select-frame current-frame))))))

(defun k0-terminate ()
  "Terminate k0 process"
  (interactive)
  (if (eq system-type 'windows-nt)
    ;; for Windows
      (let ((exec-path (k0-effective-exec-path)))
        (start-process "k0-proc-cmdk0" nil "cmdk0" "--quit"))
    ;; for UNIX
    (if (get-buffer k0-cmd-buffer-name)
        (if (not window-system)
            (let ((k0-cmd-window (get-buffer-window k0-cmd-buffer-name)))
              (and (kill-buffer k0-cmd-buffer-name)
                   (or (not k0-cmd-window) (delete-window k0-cmd-window))))
          (let ((k0-cmd-frame (window-frame (get-buffer-window k0-cmd-buffer-name 0))))
            (and (kill-buffer k0-cmd-buffer-name)
                 (delete-frame k0-cmd-frame)))))))

(defun k0-execute-current-buffer ()
  "Execute current buffer on k0"
  (interactive)
  (let ((exec-path (k0-effective-exec-path)))
    (k0-cmd-load (buffer-file-name))))

(defun k0-execute-region ()
  "Execute region on k0"
  (interactive)
  (if mark-active
      (save-excursion
        (let ((temp-file (make-temp-file (format "%s/cmdk0-" (or (getenv "TEMP") "/var/tmp"))))
              (temp-buffer (generate-new-buffer " *k0-temp*")))
          (write-region (region-beginning) (region-end) temp-file)
          (set-buffer temp-buffer)
          (insert " ;;")
          (write-region (point-min) (point-max) temp-file t) ;; append
          (kill-buffer temp-buffer)
          (k0-cmd-load temp-file)))))

(defun k0-paste-region ()
  "Paste region to k0"
  (interactive)
  (if mark-active
      (if (eq system-type 'windows-nt)
          (let ((temp-file (make-temp-file (format "%s/cmdk0-" (getenv "TEMP"))))
                (exec-path (k0-effective-exec-path)))
            (write-region (region-beginning) (region-end) temp-file)
            (start-process "k0-proc-cmdk0" nil "cmdk0" "--paste-contents" temp-file))
        (save-excursion
          (let ((buffer (current-buffer))
                (start (region-beginning))
                (end (region-end)))
            (set-buffer k0-cmd-buffer-name)
            (goto-char (point-max))
            (insert-buffer-substring buffer start end)
            (comint-send-input))))))

(defun k0-abort ()
  "Abort calculation on k0"
  (interactive)
  (if (eq system-type 'windows-nt)
    ;; for Windows
      (let ((exec-path (k0-effective-exec-path)))
        (start-process "k0-proc-cmdk0" nil "cmdk0" "--abort"))
    ;; for UNIX
    (save-excursion
      (if (get-buffer k0-cmd-buffer-name)
          (progn
            (set-buffer k0-cmd-buffer-name)
            (comint-kill-input)
            (comint-interrupt-subjob)
            (goto-char (point-max))
            (insert "t\ny")
            (comint-send-input)
            )))))

;;;; Extension for CC-mode.

(require 'cc-mode)

(eval-when-compile
  (require 'cc-langs)
  (require 'cc-engine)
  (require 'cc-fonts))

(eval-and-compile
  ;; Make our mode known to the language constant system.  Use Java
  ;; mode as the fallback for the constants we don't change here.
  ;; This needs to be done also at compile time since the language
  ;; constants are evaluated then.
  (c-add-language 'k0-mode 'c++-mode))

(c-lang-defconst c-stmt-delim-chars k0 "^;${}?:")
(c-lang-defconst c-stmt-delim-chars-with-comma k0 "^;$,{}?:")
(c-lang-defconst c-other-op-syntax-tokens 
  k0 (cons "$" (c-lang-const c-other-op-syntax-tokens c)))
(c-lang-defconst c-identifier-syntax-modifications 
  k0 (remove '(?$ . "w") (c-lang-const c-identifier-syntax-modifications c)))
(c-lang-defconst c-symbol-chars k0 (concat c-alnum "_"))

(c-lang-defconst c-primitive-type-kwds k0 '("def" "extern" "static" "localf" "function"))
(c-lang-defconst c-primitive-type-prefix-kwds k0 nil)
(c-lang-defconst c-type-list-kwds k0 nil)
(c-lang-defconst c-class-decl-kwds k0 '("module"))
(c-lang-defconst c-othe-decl-kwds  k0 '("endmodule" "end"))
(c-lang-defconst c-type-modifier-kwds k0 nil)
(c-lang-defconst c-modifier-kwds k0 nil)

(c-lang-defconst c-mode-menu 
  k0
  (append (c-lang-const c-mode-menu c)
		  '("----" 
			["Start K0" k0-start t]
			["Terminate K0" k0-terminate t]
			["Abort calcuration on K0" k0-abort t]
			["Execute Current Buffer on K0" k0-execute-current-buffer (buffer-file-name)]
			["Execute Region on K0" k0-execute-region mark-active]
			["Paste Region to K0" k0-paste-region mark-active]
			)))

(defvar k0-font-lock-extra-types nil
  "*List of extra types (aside from the type keywords) to recognize in k0 mode.
Each list item should be a regexp matching a single identifier.")

(defconst k0-font-lock-keywords-1 (c-lang-const c-matchers-1 k0)
  "Minimal highlighting for k0 mode.")

(defconst k0-font-lock-keywords-2 (c-lang-const c-matchers-2 k0)
  "Fast normal highlighting for k0 mode.")

(defconst k0-font-lock-keywords-3 (c-lang-const c-matchers-3 k0)
  "Accurate normal highlighting for k0 mode.")

(defvar k0-font-lock-keywords k0-font-lock-keywords-3
  "Default expressions to highlight in k0 mode.")

(defvar k0-mode-syntax-table nil
  "Syntax table used in k0-mode buffers.")
(or k0-mode-syntax-table
    (setq k0-mode-syntax-table
	  (funcall (c-lang-const c-make-mode-syntax-table k0))))

(defvar k0-mode-abbrev-table nil
  "Abbreviation table used in k0-mode buffers.")

(defvar k0-mode-map (let ((map (c-make-inherited-keymap)))
		      ;; Add bindings which are only useful for k0
		      map)
  "Keymap used in k0-mode buffers.")

;; Key binding for k0-mode
(define-key k0-mode-map (kbd "C-c s") 'k0-start)
(define-key k0-mode-map (kbd "C-c t") 'k0-terminate)
(define-key k0-mode-map (kbd "C-c a") 'k0-abort)
(define-key k0-mode-map (kbd "C-c l") 'k0-execute-current-buffer)
(define-key k0-mode-map (kbd "C-c r") 'k0-execute-region)
(define-key k0-mode-map (kbd "C-c p") 'k0-paste-region)

(easy-menu-define k0-menu k0-mode-map "k0 Mode Commands"
		  ;; Can use `k0' as the language for `c-mode-menu'
		  ;; since its definition covers any language.  In
		  ;; this case the language is used to adapt to the
		  ;; nonexistence of a cpp pass and thus removing some
		  ;; irrelevant menu alternatives.
		  (cons "K0" (c-lang-const c-mode-menu k0)))

(defun k0-mode ()
  "Major mode for editing k0 code.
This is a simple example of a separate mode derived from CC Mode to
support a language with syntax similar to C/C++/ObjC/Java/IDL/Pike.

The hook `c-mode-common-hook' is run with no args at mode
initialization, then `k0-mode-hook'.

Key bindings:
\\{k0-mode-map}"
  (interactive)
  (kill-all-local-variables)
  (c-initialize-cc-mode t)
  (set-syntax-table k0-mode-syntax-table)
  (setq major-mode 'k0-mode
	mode-name "k0"
	local-abbrev-table k0-mode-abbrev-table
	abbrev-mode t)
  (use-local-map k0-mode-map)
  ;; `c-init-language-vars' is a macro that is expanded at compile
  ;; time to a large `setq' with all the language variables and their
  ;; customized values for our language.
  (c-init-language-vars k0-mode)
  ;; `c-common-init' initializes most of the components of a CC Mode
  ;; buffer, including setup of the mode menu, font-lock, etc.
  ;; There's also a lower level routine `c-basic-common-init' that
  ;; only makes the necessary initialization to get the syntactic
  ;; analysis and similar things working.
  (c-common-init 'k0-mode)
;;(easy-menu-add k0-menu)
  (run-hooks 'c-mode-common-hook)
  (run-hooks 'k0-mode-hook)
  (c-update-modeline))

(if (fboundp 'k0-backup:c-guess-basic-syntax)
	nil
  (fset 'k0-backup:c-guess-basic-syntax (symbol-function 'c-guess-basic-syntax))
  (defun c-guess-basic-syntax ()
	"A modified c-guess-basic-syntax for k0-mode"
	(if (eq major-mode 'k0-mode)
		(k0-c-guess-basic-syntax)
	  (k0-backup:c-guess-basic-syntax))))

;; Meadow 3 does not have `c-brace-anchor-point'
;; This function was copied from cc-engine.el of Emacs 23.4
(if (and (featurep 'meadow) (not (fboundp 'c-brace-anchor-point)))
    (defun c-brace-anchor-point (bracepos)
      ;; BRACEPOS is the position of a brace in a construct like "namespace
      ;; Bar {".  Return the anchor point in this construct; this is the
      ;; earliest symbol on the brace's line which isn't earlier than
      ;; "namespace".
      ;;
      ;; Currently (2007-08-17), "like namespace" means "matches
      ;; c-other-block-decl-kwds".  It doesn't work with "class" or "struct"
      ;; or anything like that.
      (save-excursion
        (let ((boi (c-point 'boi bracepos)))
          (goto-char bracepos)
          (while (and (> (point) boi)
                      (not (looking-at c-other-decl-block-key)))
            (c-backward-token-2))
          (if (> (point) boi) (point) boi))))
  )

;; The function `c-guess-basic-syntax' was copied from cc-engine.el of Emacs 23.4 and 
;; was modified for Risa/K0.
;; CASE 5D, 5J, 18 are corrected.

;;;; Beginning of `k0-c-guess-basic-syntax'
(defun k0-c-guess-basic-syntax ()
  "Return the syntactic context of the current line."
  (save-excursion
      (beginning-of-line)
      (c-save-buffer-state
	  ((indent-point (point))
	   (case-fold-search nil)
	   ;; A whole ugly bunch of various temporary variables.  Have
	   ;; to declare them here since it's not possible to declare
	   ;; a variable with only the scope of a cond test and the
	   ;; following result clauses, and most of this function is a
	   ;; single gigantic cond. :P
	   literal char-before-ip before-ws-ip char-after-ip macro-start
	   in-macro-expr c-syntactic-context placeholder c-in-literal-cache
	   step-type tmpsymbol keyword injava-inher special-brace-list tmp-pos
	   containing-<
	   ;; The following record some positions for the containing
	   ;; declaration block if we're directly within one:
	   ;; `containing-decl-open' is the position of the open
	   ;; brace.  `containing-decl-start' is the start of the
	   ;; declaration.  `containing-decl-kwd' is the keyword
	   ;; symbol of the keyword that tells what kind of block it
	   ;; is.
	   containing-decl-open
	   containing-decl-start
	   containing-decl-kwd
	   ;; The open paren of the closest surrounding sexp or nil if
	   ;; there is none.
	   containing-sexp
	   ;; The position after the closest preceding brace sexp
	   ;; (nested sexps are ignored), or the position after
	   ;; `containing-sexp' if there is none, or (point-min) if
	   ;; `containing-sexp' is nil.
	   lim
	   ;; The paren state outside `containing-sexp', or at
	   ;; `indent-point' if `containing-sexp' is nil.
	   (paren-state (c-parse-state))
	   ;; There's always at most one syntactic element which got
	   ;; an anchor pos.  It's stored in syntactic-relpos.
	   syntactic-relpos
	   (c-stmt-delim-chars c-stmt-delim-chars))

	;; Check if we're directly inside an enclosing declaration
	;; level block.
	(when (and (setq containing-sexp
			 (c-most-enclosing-brace paren-state))
		   (progn
		     (goto-char containing-sexp)
		     (eq (char-after) ?{))
		   (setq placeholder
			 (c-looking-at-decl-block
			  (c-most-enclosing-brace paren-state
						  containing-sexp)
			  t)))
	  (setq containing-decl-open containing-sexp
		containing-decl-start (point)
		containing-sexp nil)
	  (goto-char placeholder)
	  (setq containing-decl-kwd (and (looking-at c-keywords-regexp)
					 (c-keyword-sym (match-string 1)))))

	;; Init some position variables.
	(if c-state-cache
	    (progn
	      (setq containing-sexp (car paren-state)
		    paren-state (cdr paren-state))
	      (if (consp containing-sexp)
		  (progn
		    (setq lim (cdr containing-sexp))
		    (if (cdr c-state-cache)
			;; Ignore balanced paren.  The next entry
			;; can't be another one.
			(setq containing-sexp (car (cdr c-state-cache))
			      paren-state (cdr paren-state))
		      ;; If there is no surrounding open paren then
		      ;; put the last balanced pair back on paren-state.
		      (setq paren-state (cons containing-sexp paren-state)
			    containing-sexp nil)))
		(setq lim (1+ containing-sexp))))
	  (setq lim (point-min)))

	;; If we're in a parenthesis list then ',' delimits the
	;; "statements" rather than being an operator (with the
	;; exception of the "for" clause).  This difference is
	;; typically only noticeable when statements are used in macro
	;; arglists.
	(when (and containing-sexp
		   (eq (char-after containing-sexp) ?\())
	  (setq c-stmt-delim-chars c-stmt-delim-chars-with-comma))

	;; cache char before and after indent point, and move point to
	;; the most likely position to perform the majority of tests
	(goto-char indent-point)
	(c-backward-syntactic-ws lim)
	(setq before-ws-ip (point)
	      char-before-ip (char-before))
	(goto-char indent-point)
	(skip-chars-forward " \t")
	(setq char-after-ip (char-after))

	;; are we in a literal?
	(setq literal (c-in-literal lim))

	;; now figure out syntactic qualities of the current line
	(cond

	 ;; CASE 1: in a string.
	 ((eq literal 'string)
	  (c-add-syntax 'string (c-point 'bopl)))

	 ;; CASE 2: in a C or C++ style comment.
	 ((and (memq literal '(c c++))
	       ;; This is a kludge for XEmacs where we use
	       ;; `buffer-syntactic-context', which doesn't correctly
	       ;; recognize "\*/" to end a block comment.
	       ;; `parse-partial-sexp' which is used by
	       ;; `c-literal-limits' will however do that in most
	       ;; versions, which results in that we get nil from
	       ;; `c-literal-limits' even when `c-in-literal' claims
	       ;; we're inside a comment.
	       (setq placeholder (c-literal-limits lim)))
	  (c-add-syntax literal (car placeholder)))

	 ;; CASE 3: in a cpp preprocessor macro continuation.
	 ((and (save-excursion
		 (when (c-beginning-of-macro)
		   (setq macro-start (point))))
	       (/= macro-start (c-point 'boi))
	       (progn
		 (setq tmpsymbol 'cpp-macro-cont)
		 (or (not c-syntactic-indentation-in-macros)
		     (save-excursion
		       (goto-char macro-start)
		       ;; If at the beginning of the body of a #define
		       ;; directive then analyze as cpp-define-intro
		       ;; only.  Go on with the syntactic analysis
		       ;; otherwise.  in-macro-expr is set if we're in a
		       ;; cpp expression, i.e. before the #define body
		       ;; or anywhere in a non-#define directive.
		       (if (c-forward-to-cpp-define-body)
			   (let ((indent-boi (c-point 'boi indent-point)))
			     (setq in-macro-expr (> (point) indent-boi)
				   tmpsymbol 'cpp-define-intro)
			     (= (point) indent-boi))
			 (setq in-macro-expr t)
			 nil)))))
	  (c-add-syntax tmpsymbol macro-start)
	  (setq macro-start nil))

	 ;; CASE 11: an else clause?
	 ((looking-at "else\\>[^_]")
	  (c-beginning-of-statement-1 containing-sexp)
	  (c-add-stmt-syntax 'else-clause nil t
			     containing-sexp paren-state))

	 ;; CASE 12: while closure of a do/while construct?
	 ((and (looking-at "while\\>[^_]")
	       (save-excursion
		 (prog1 (eq (c-beginning-of-statement-1 containing-sexp)
			    'beginning)
		   (setq placeholder (point)))))
	  (goto-char placeholder)
	  (c-add-stmt-syntax 'do-while-closure nil t
			     containing-sexp paren-state))

	 ;; CASE 13: A catch or finally clause?  This case is simpler
	 ;; than if-else and do-while, because a block is required
	 ;; after every try, catch and finally.
	 ((save-excursion
	    (and (cond ((c-major-mode-is 'c++-mode)
			(looking-at "catch\\>[^_]"))
		       ((c-major-mode-is 'java-mode)
			(looking-at "\\(catch\\|finally\\)\\>[^_]")))
		 (and (c-safe (c-backward-syntactic-ws)
			      (c-backward-sexp)
			      t)
		      (eq (char-after) ?{)
		      (c-safe (c-backward-syntactic-ws)
			      (c-backward-sexp)
			      t)
		      (if (eq (char-after) ?\()
			  (c-safe (c-backward-sexp) t)
			t))
		 (looking-at "\\(try\\|catch\\)\\>[^_]")
		 (setq placeholder (point))))
	  (goto-char placeholder)
	  (c-add-stmt-syntax 'catch-clause nil t
			     containing-sexp paren-state))

	 ;; CASE 18: A substatement we can recognize by keyword.
	 ((save-excursion
	    (and c-opt-block-stmt-key
		 (not (eq char-before-ip ?\;))
		 (not (c-at-vsemi-p before-ws-ip))
		 (not (memq char-after-ip '(?\) ?\] ?,)))
		 (or (not (eq char-before-ip ?}))
		     (c-looking-at-inexpr-block-backward c-state-cache))
		 (> (point)
		    (progn
		      ;; Ought to cache the result from the
		      ;; c-beginning-of-statement-1 calls here.
		      (setq placeholder (point))
		      (while (eq (setq step-type
				       (c-beginning-of-statement-1 lim))
				 'label))
		      (if (eq step-type 'previous)
			  (goto-char placeholder)
			(setq placeholder (point))
			(if (and (eq step-type 'same)
				 (not (looking-at c-opt-block-stmt-key)))
			    ;; Step up to the containing statement if we
			    ;; stayed in the same one.
			    (let (step)
			      (while (eq
				      (setq step
					    (c-beginning-of-statement-1 lim))
				      'label))
			      (if (eq step 'up)
				  (setq placeholder (point))
				;; There was no containing statement after all.
				(goto-char placeholder)))))
		      placeholder))
		 (if (looking-at c-block-stmt-2-key)
		     ;; Require a parenthesis after these keywords.
		     ;; Necessary to catch e.g. synchronized in Java,
		     ;; which can be used both as statement and
		     ;; modifier.
		     (and (zerop (c-forward-token-2 1 nil))
			  (eq (char-after) ?\())
		   (looking-at c-opt-block-stmt-key))))

	  (if (eq step-type 'up)
	      ;; CASE 18A: Simple substatement.
	      (progn
		(goto-char placeholder)
		(cond
		 ((eq char-after-ip ?{)
		  (c-add-stmt-syntax 'substatement-open nil nil
				     containing-sexp paren-state))
		 ((save-excursion
		    (goto-char indent-point)
		    (back-to-indentation)
		    (c-forward-label))
		  (c-add-stmt-syntax 'substatement-label nil nil
				     containing-sexp paren-state))
		 (t
		  (c-add-stmt-syntax 'substatement nil nil
				     containing-sexp paren-state))))

	    ;; CASE 18B: Some other substatement.  This is shared
	    ;; with case 10.
	    (c-guess-continued-construct indent-point
					 char-after-ip
					 placeholder
					 lim
					 paren-state)))

	 ;; CASE 14: A case or default label
	 ((looking-at c-label-kwds-regexp)
	  (if containing-sexp
	      (progn
		(goto-char containing-sexp)
		(setq lim (c-most-enclosing-brace c-state-cache
						  containing-sexp))
		(c-backward-to-block-anchor lim)
		(c-add-stmt-syntax 'case-label nil t lim paren-state))
	    ;; Got a bogus label at the top level.  In lack of better
	    ;; alternatives, anchor it on (point-min).
	    (c-add-syntax 'case-label (point-min))))

	 ;; CASE 15: any other label
	 ((save-excursion
	    (back-to-indentation)
	    (and (not (looking-at c-syntactic-ws-start))
		 (c-forward-label)))
	  (cond (containing-decl-open
		 (setq placeholder (c-add-class-syntax 'inclass
						       containing-decl-open
						       containing-decl-start
						       containing-decl-kwd
						       paren-state))
		 ;; Append access-label with the same anchor point as
		 ;; inclass gets.
		 (c-append-syntax 'access-label placeholder))

		(containing-sexp
		 (goto-char containing-sexp)
		 (setq lim (c-most-enclosing-brace c-state-cache
						   containing-sexp))
		 (save-excursion
		   (setq tmpsymbol
			 (if (and (eq (c-beginning-of-statement-1 lim) 'up)
				  (looking-at "switch\\>[^_]"))
			     ;; If the surrounding statement is a switch then
			     ;; let's analyze all labels as switch labels, so
			     ;; that they get lined up consistently.
			     'case-label
			   'label)))
		 (c-backward-to-block-anchor lim)
		 (c-add-stmt-syntax tmpsymbol nil t lim paren-state))

		(t
		 ;; A label on the top level.  Treat it as a class
		 ;; context.  (point-min) is the closest we get to the
		 ;; class open brace.
		 (c-add-syntax 'access-label (point-min)))))

	 ;; CASE 4: In-expression statement.  C.f. cases 7B, 16A and
	 ;; 17E.
	 ((setq placeholder (c-looking-at-inexpr-block
			     (c-safe-position containing-sexp paren-state)
			     containing-sexp
			     ;; Have to turn on the heuristics after
			     ;; the point even though it doesn't work
			     ;; very well.  C.f. test case class-16.pike.
			     t))
	  (setq tmpsymbol (assq (car placeholder)
				'((inexpr-class . class-open)
				  (inexpr-statement . block-open))))
	  (if tmpsymbol
	      ;; It's a statement block or an anonymous class.
	      (setq tmpsymbol (cdr tmpsymbol))
	    ;; It's a Pike lambda.  Check whether we are between the
	    ;; lambda keyword and the argument list or at the defun
	    ;; opener.
	    (setq tmpsymbol (if (eq char-after-ip ?{)
				'inline-open
			      'lambda-intro-cont)))
	  (goto-char (cdr placeholder))
	  (back-to-indentation)
	  (c-add-stmt-syntax tmpsymbol nil t
			     (c-most-enclosing-brace c-state-cache (point))
			     paren-state)
	  (unless (eq (point) (cdr placeholder))
	    (c-add-syntax (car placeholder))))

	 ;; CASE 5: Line is inside a declaration level block or at top level.
	 ((or containing-decl-open (null containing-sexp))
	  (cond

	   ;; CASE 5A: we are looking at a defun, brace list, class,
	   ;; or inline-inclass method opening brace
	   ((setq special-brace-list
		  (or (and c-special-brace-lists
			   (c-looking-at-special-brace-list))
		      (eq char-after-ip ?{)))
	    (cond

	     ;; CASE 5A.1: Non-class declaration block open.
	     ((save-excursion
		(let (tmp)
		  (and (eq char-after-ip ?{)
		       (setq tmp (c-looking-at-decl-block containing-sexp t))
		       (progn
			 (setq placeholder (point))
			 (goto-char tmp)
			 (looking-at c-symbol-key))
		       (c-keyword-member
			(c-keyword-sym (setq keyword (match-string 0)))
			'c-other-block-decl-kwds))))
	      (goto-char placeholder)
	      (c-add-stmt-syntax
	       (if (string-equal keyword "extern")
		   ;; Special case for extern-lang-open.
		   'extern-lang-open
		 (intern (concat keyword "-open")))
	       nil t containing-sexp paren-state))

	     ;; CASE 5A.2: we are looking at a class opening brace
	     ((save-excursion
		(goto-char indent-point)
		(skip-chars-forward " \t")
		(and (eq (char-after) ?{)
		     (c-looking-at-decl-block containing-sexp t)
		     (setq placeholder (point))))
	      (c-add-syntax 'class-open placeholder))

	     ;; CASE 5A.3: brace list open
	     ((save-excursion
		(c-beginning-of-decl-1 lim)
		(while (looking-at c-specifier-key)
		  (goto-char (match-end 1))
		  (c-forward-syntactic-ws indent-point))
		(setq placeholder (c-point 'boi))
		(or (consp special-brace-list)
		    (and (or (save-excursion
			       (goto-char indent-point)
			       (setq tmpsymbol nil)
			       (while (and (> (point) placeholder)
					   (zerop (c-backward-token-2 1 t))
					   (/= (char-after) ?=))
				 (and c-opt-inexpr-brace-list-key
				      (not tmpsymbol)
				      (looking-at c-opt-inexpr-brace-list-key)
				      (setq tmpsymbol 'topmost-intro-cont)))
			       (eq (char-after) ?=))
			     (looking-at c-brace-list-key))
			 (save-excursion
			   (while (and (< (point) indent-point)
				       (zerop (c-forward-token-2 1 t))
				       (not (memq (char-after) '(?\; ?\()))))
			   (not (memq (char-after) '(?\; ?\()))
			   ))))
	      (if (and (not c-auto-newline-analysis)
		       (c-major-mode-is 'java-mode)
		       (eq tmpsymbol 'topmost-intro-cont))
		  ;; We're in Java and have found that the open brace
		  ;; belongs to a "new Foo[]" initialization list,
		  ;; which means the brace list is part of an
		  ;; expression and not a top level definition.  We
		  ;; therefore treat it as any topmost continuation
		  ;; even though the semantically correct symbol still
		  ;; is brace-list-open, on the same grounds as in
		  ;; case B.2.
		  (progn
		    (c-beginning-of-statement-1 lim)
		    (c-add-syntax 'topmost-intro-cont (c-point 'boi)))
		(c-add-syntax 'brace-list-open placeholder)))

	     ;; CASE 5A.4: inline defun open
	     ((and containing-decl-open
		   (not (c-keyword-member containing-decl-kwd
					  'c-other-block-decl-kwds)))
	      (c-add-syntax 'inline-open)
	      (c-add-class-syntax 'inclass
				  containing-decl-open
				  containing-decl-start
				  containing-decl-kwd
				  paren-state))

	     ;; CASE 5A.5: ordinary defun open
	     (t
	      (save-excursion
		(c-beginning-of-decl-1 lim)
		(while (looking-at c-specifier-key)
		  (goto-char (match-end 1))
		  (c-forward-syntactic-ws indent-point))
		(c-add-syntax 'defun-open (c-point 'boi))
		;; Bogus to use bol here, but it's the legacy.  (Resolved,
		;; 2007-11-09)
		))))

	   ;; CASE 5B: After a function header but before the body (or
	   ;; the ending semicolon if there's no body).
	   ((save-excursion
	      (when (setq placeholder (c-just-after-func-arglist-p lim))
		(setq tmp-pos (point))))
	    (cond

	     ;; CASE 5B.1: Member init list.
	     ((eq (char-after tmp-pos) ?:)
	      (if (or (> tmp-pos indent-point)
		      (= (c-point 'bosws) (1+ tmp-pos)))
		  (progn
		    ;; There is no preceding member init clause.
		    ;; Indent relative to the beginning of indentation
		    ;; for the topmost-intro line that contains the
		    ;; prototype's open paren.
		    (goto-char placeholder)
		    (c-add-syntax 'member-init-intro (c-point 'boi)))
		;; Indent relative to the first member init clause.
		(goto-char (1+ tmp-pos))
		(c-forward-syntactic-ws)
		(c-add-syntax 'member-init-cont (point))))

	     ;; CASE 5B.2: K&R arg decl intro
	     ((and c-recognize-knr-p
		   (c-in-knr-argdecl lim))
	      (c-beginning-of-statement-1 lim)
	      (c-add-syntax 'knr-argdecl-intro (c-point 'boi))
	      (if containing-decl-open
		  (c-add-class-syntax 'inclass
				      containing-decl-open
				      containing-decl-start
				      containing-decl-kwd
				      paren-state)))

	     ;; CASE 5B.4: Nether region after a C++ or Java func
	     ;; decl, which could include a `throws' declaration.
	     (t
	      (c-beginning-of-statement-1 lim)
	      (c-add-syntax 'func-decl-cont (c-point 'boi))
	      )))

	   ;; CASE 5C: inheritance line. could be first inheritance
	   ;; line, or continuation of a multiple inheritance
	   ((or (and (c-major-mode-is 'c++-mode)
		     (progn
		       (when (eq char-after-ip ?,)
			 (skip-chars-forward " \t")
			 (forward-char))
		       (looking-at c-opt-postfix-decl-spec-key)))
		(and (or (eq char-before-ip ?:)
			 ;; watch out for scope operator
			 (save-excursion
			   (and (eq char-after-ip ?:)
				(c-safe (forward-char 1) t)
				(not (eq (char-after) ?:))
				)))
		     (save-excursion
		       (c-backward-syntactic-ws lim)
		       (if (eq char-before-ip ?:)
			   (progn
			     (forward-char -1)
			     (c-backward-syntactic-ws lim)))
		       (back-to-indentation)
		       (looking-at c-class-key)))
		;; for Java
		(and (c-major-mode-is 'java-mode)
		     (let ((fence (save-excursion
				    (c-beginning-of-statement-1 lim)
				    (point)))
			   cont done)
		       (save-excursion
			 (while (not done)
			   (cond ((looking-at c-opt-postfix-decl-spec-key)
				  (setq injava-inher (cons cont (point))
					done t))
				 ((or (not (c-safe (c-forward-sexp -1) t))
				      (<= (point) fence))
				  (setq done t))
				 )
			   (setq cont t)))
		       injava-inher)
		     (not (c-crosses-statement-barrier-p (cdr injava-inher)
							 (point)))
		     ))
	    (cond

	     ;; CASE 5C.1: non-hanging colon on an inher intro
	     ((eq char-after-ip ?:)
	      (c-beginning-of-statement-1 lim)
	      (c-add-syntax 'inher-intro (c-point 'boi))
	      ;; don't add inclass symbol since relative point already
	      ;; contains any class offset
	      )

	     ;; CASE 5C.2: hanging colon on an inher intro
	     ((eq char-before-ip ?:)
	      (c-beginning-of-statement-1 lim)
	      (c-add-syntax 'inher-intro (c-point 'boi))
	      (if containing-decl-open
		  (c-add-class-syntax 'inclass
				      containing-decl-open
				      containing-decl-start
				      containing-decl-kwd
				      paren-state)))

	     ;; CASE 5C.3: in a Java implements/extends
	     (injava-inher
	      (let ((where (cdr injava-inher))
		    (cont (car injava-inher)))
		(goto-char where)
		(cond ((looking-at "throws\\>[^_]")
		       (c-add-syntax 'func-decl-cont
				     (progn (c-beginning-of-statement-1 lim)
					    (c-point 'boi))))
		      (cont (c-add-syntax 'inher-cont where))
		      (t (c-add-syntax 'inher-intro
				       (progn (goto-char (cdr injava-inher))
					      (c-beginning-of-statement-1 lim)
					      (point))))
		      )))

	     ;; CASE 5C.4: a continued inheritance line
	     (t
	      (c-beginning-of-inheritance-list lim)
	      (c-add-syntax 'inher-cont (point))
	      ;; don't add inclass symbol since relative point already
	      ;; contains any class offset
	      )))

	   ;; CASE 5D: this could be a top-level initialization, a
	   ;; member init list continuation, or a template argument
	   ;; list continuation.
	   ((save-excursion
	      ;; Note: We use the fact that lim is always after any
	      ;; preceding brace sexp.
	      (if c-recognize-<>-arglists
		  (while (and
			  (progn
			    (c-syntactic-skip-backward "^;$,=<>" lim t)
			    (> (point) lim))
			  (or
			   (when c-overloadable-operators-regexp
			     (when (setq placeholder (c-after-special-operator-id lim))
			       (goto-char placeholder)
			       t))
			   (cond
			    ((eq (char-before) ?>)
			     (or (c-backward-<>-arglist nil lim)
				 (backward-char))
			     t)
			    ((eq (char-before) ?<)
			     (backward-char)
			     (if (save-excursion
				   (c-forward-<>-arglist nil))
				 (progn (forward-char)
					nil)
			       t))
			    (t nil)))))
		;; NB: No c-after-special-operator-id stuff in this
		;; clause - we assume only C++ needs it.
		(c-syntactic-skip-backward "^;$,=" lim t))
	      (memq (char-before) '(?, ?= ?<)))
	    (cond

	     ;; CASE 5D.3: perhaps a template list continuation?
	     ((and (c-major-mode-is 'c++-mode)
		   (save-excursion
		     (save-restriction
		       (c-with-syntax-table c++-template-syntax-table
			 (goto-char indent-point)
			 (setq placeholder (c-up-list-backward))
			 (and placeholder
			      (eq (char-after placeholder) ?<))))))
	      (c-with-syntax-table c++-template-syntax-table
		(goto-char placeholder)
		(c-beginning-of-statement-1 lim t)
		(if (save-excursion
		      (c-backward-syntactic-ws lim)
		      (eq (char-before) ?<))
		    ;; In a nested template arglist.
		    (progn
		      (goto-char placeholder)
		      (c-syntactic-skip-backward "^,;" lim t)
		      (c-forward-syntactic-ws))
		  (back-to-indentation)))
	      ;; FIXME: Should use c-add-stmt-syntax, but it's not yet
	      ;; template aware.
	      (c-add-syntax 'template-args-cont (point) placeholder))

	     ;; CASE 5D.4: perhaps a multiple inheritance line?
	     ((and (c-major-mode-is 'c++-mode)
		   (save-excursion
		     (c-beginning-of-statement-1 lim)
		     (setq placeholder (point))
		     (if (looking-at "static\\>[^_]")
			 (c-forward-token-2 1 nil indent-point))
		     (and (looking-at c-class-key)
			  (zerop (c-forward-token-2 2 nil indent-point))
			  (if (eq (char-after) ?<)
			      (c-with-syntax-table c++-template-syntax-table
				(zerop (c-forward-token-2 1 t indent-point)))
			    t)
			  (eq (char-after) ?:))))
	      (goto-char placeholder)
	      (c-add-syntax 'inher-cont (c-point 'boi)))

	     ;; CASE 5D.5: Continuation of the "expression part" of a
	     ;; top level construct.  Or, perhaps, an unrecognized construct.
	     (t
	      (while (and (setq placeholder (point))
			  (eq (car (c-beginning-of-decl-1 containing-sexp))
			      'same)
			  (save-excursion
			    (c-backward-syntactic-ws)
			    (eq (char-before) ?}))
			  (< (point) placeholder)))
	      (c-add-stmt-syntax
	       (cond
		((eq (point) placeholder) 'statement) ; unrecognized construct
		   ;; A preceding comma at the top level means that a
		   ;; new variable declaration starts here.  Use
		   ;; topmost-intro-cont for it, for consistency with
		   ;; the first variable declaration.  C.f. case 5N.
		((eq char-before-ip ?,) 'topmost-intro-cont)
		(t 'statement-cont))
	       nil nil containing-sexp paren-state))
	     ))

	   ;; CASE 5F: Close of a non-class declaration level block.
	   ((and (eq char-after-ip ?})
		 (c-keyword-member containing-decl-kwd
				   'c-other-block-decl-kwds))
	    ;; This is inconsistent: Should use `containing-decl-open'
	    ;; here if it's at boi, like in case 5J.
	    (goto-char containing-decl-start)
	    (c-add-stmt-syntax
	      (if (string-equal (symbol-name containing-decl-kwd) "extern")
		  ;; Special case for compatibility with the
		  ;; extern-lang syntactic symbols.
		  'extern-lang-close
		(intern (concat (symbol-name containing-decl-kwd)
				"-close")))
	      nil t
	      (c-most-enclosing-brace paren-state (point))
	      paren-state))

	   ;; CASE 5G: we are looking at the brace which closes the
	   ;; enclosing nested class decl
	   ((and containing-sexp
		 (eq char-after-ip ?})
		 (eq containing-decl-open containing-sexp))
	    (c-add-class-syntax 'class-close
				containing-decl-open
				containing-decl-start
				containing-decl-kwd
				paren-state))

	   ;; CASE 5H: we could be looking at subsequent knr-argdecls
	   ((and c-recognize-knr-p
		 (not containing-sexp)	; can't be knr inside braces.
		 (not (eq char-before-ip ?}))
		 (save-excursion
		   (setq placeholder (cdr (c-beginning-of-decl-1 lim)))
		   (and placeholder
			;; Do an extra check to avoid tripping up on
			;; statements that occur in invalid contexts
			;; (e.g. in macro bodies where we don't really
			;; know the context of what we're looking at).
			(not (and c-opt-block-stmt-key
				  (looking-at c-opt-block-stmt-key)))))
		 (< placeholder indent-point))
	    (goto-char placeholder)
	    (c-add-syntax 'knr-argdecl (point)))

	   ;; CASE 5I: ObjC method definition.
	   ((and c-opt-method-key
		 (looking-at c-opt-method-key))
	    (c-beginning-of-statement-1 nil t)
	    (if (= (point) indent-point)
		;; Handle the case when it's the first (non-comment)
		;; thing in the buffer.  Can't look for a 'same return
		;; value from cbos1 since ObjC directives currently
		;; aren't recognized fully, so that we get 'same
		;; instead of 'previous if it moved over a preceding
		;; directive.
		(goto-char (point-min)))
	    (c-add-syntax 'objc-method-intro (c-point 'boi)))

           ;; CASE 5P: AWK pattern or function or continuation
           ;; thereof.
           ((c-major-mode-is 'awk-mode)
            (setq placeholder (point))
            (c-add-stmt-syntax
             (if (and (eq (c-beginning-of-statement-1) 'same)
                      (/= (point) placeholder))
                 'topmost-intro-cont
               'topmost-intro)
             nil nil
             containing-sexp paren-state))

	   ;; CASE 5N: At a variable declaration that follows a class
	   ;; definition or some other block declaration that doesn't
	   ;; end at the closing '}'.  C.f. case 5D.5.
	   ((progn
	      (c-backward-syntactic-ws lim)
	      (and (eq (char-before) ?})
		   (save-excursion
		     (let ((start (point)))
		       (if (and c-state-cache
				(consp (car c-state-cache))
				(eq (cdar c-state-cache) (point)))
			   ;; Speed up the backward search a bit.
			   (goto-char (caar c-state-cache)))
		       (c-beginning-of-decl-1 containing-sexp)
		       (setq placeholder (point))
		       (if (= start (point))
			   ;; The '}' is unbalanced.
			   nil
			 (c-end-of-decl-1)
			 (>= (point) indent-point))))))
	    (goto-char placeholder)
	    (c-add-stmt-syntax 'topmost-intro-cont nil nil
			       containing-sexp paren-state))

	   ;; NOTE: The point is at the end of the previous token here.

	   ;; CASE 5J: we are at the topmost level, make
	   ;; sure we skip back past any access specifiers
	   ((and
	     ;; A macro continuation line is never at top level.
	     (not (and macro-start
		       (> indent-point macro-start)))
	     (save-excursion
	       (setq placeholder (point))
	       (or (memq char-before-ip '(?\; ?$ ?{ ?} nil))
		   (c-at-vsemi-p before-ws-ip)
		   (when (and (eq char-before-ip ?:)
			      (eq (c-beginning-of-statement-1 lim)
				  'label))
		     (c-backward-syntactic-ws lim)
		     (setq placeholder (point)))
		   (and (c-major-mode-is 'objc-mode)
			(catch 'not-in-directive
			  (c-beginning-of-statement-1 lim)
			  (setq placeholder (point))
			  (while (and (c-forward-objc-directive)
				      (< (point) indent-point))
			    (c-forward-syntactic-ws)
			    (if (>= (point) indent-point)
				(throw 'not-in-directive t))
			    (setq placeholder (point)))
			  nil)))))
	    ;; For historic reasons we anchor at bol of the last
	    ;; line of the previous declaration.  That's clearly
	    ;; highly bogus and useless, and it makes our lives hard
	    ;; to remain compatible.  :P
	    (goto-char placeholder)
	    (c-add-syntax 'topmost-intro (c-point 'bol))
	    (if containing-decl-open
		(if (c-keyword-member containing-decl-kwd
				      'c-other-block-decl-kwds)
		    (progn
		      (goto-char (c-brace-anchor-point containing-decl-open))
		      (c-add-stmt-syntax
		       (if (string-equal (symbol-name containing-decl-kwd)
					 "extern")
			   ;; Special case for compatibility with the
			   ;; extern-lang syntactic symbols.
			   'inextern-lang
			 (intern (concat "in"
					 (symbol-name containing-decl-kwd))))
		       nil t
		       (c-most-enclosing-brace paren-state (point))
		       paren-state))
		  (c-add-class-syntax 'inclass
				      containing-decl-open
				      containing-decl-start
				      containing-decl-kwd
				      paren-state)))
	    (when (and c-syntactic-indentation-in-macros
		       macro-start
		       (/= macro-start (c-point 'boi indent-point)))
	      (c-add-syntax 'cpp-define-intro)
	      (setq macro-start nil)))

	   ;; CASE 5K: we are at an ObjC method definition
	   ;; continuation line.
	   ((and c-opt-method-key
		 (save-excursion
		   (c-beginning-of-statement-1 lim)
		   (beginning-of-line)
		   (when (looking-at c-opt-method-key)
		     (setq placeholder (point)))))
	    (c-add-syntax 'objc-method-args-cont placeholder))

	   ;; CASE 5L: we are at the first argument of a template
	   ;; arglist that begins on the previous line.
	   ((and c-recognize-<>-arglists
		 (eq (char-before) ?<)
		 (setq placeholder (1- (point)))
		 (not (and c-overloadable-operators-regexp
			   (c-after-special-operator-id lim))))
	    (c-beginning-of-statement-1 (c-safe-position (point) paren-state))
	    (c-add-syntax 'template-args-cont (c-point 'boi) placeholder))

	   ;; CASE 5Q: we are at a statement within a macro.
	   (macro-start
	    (c-beginning-of-statement-1 containing-sexp)
	    (c-add-stmt-syntax 'statement nil t containing-sexp paren-state))

	   ;; CASE 5M: we are at a topmost continuation line
	   (t
	    (c-beginning-of-statement-1 (c-safe-position (point) paren-state))
	    (when (c-major-mode-is 'objc-mode)
	      (setq placeholder (point))
	      (while (and (c-forward-objc-directive)
			  (< (point) indent-point))
		(c-forward-syntactic-ws)
		(setq placeholder (point)))
	      (goto-char placeholder))
	    (c-add-syntax 'topmost-intro-cont (c-point 'boi)))
	   ))

	 ;; (CASE 6 has been removed.)

	 ;; CASE 19: line is an expression, not a statement, and is directly
	 ;; contained by a template delimiter.  Most likely, we are in a
	 ;; template arglist within a statement.  This case is based on CASE
	 ;; 7.  At some point in the future, we may wish to create more
	 ;; syntactic symbols such as `template-intro',
	 ;; `template-cont-nonempty', etc., and distinguish between them as we
	 ;; do for `arglist-intro' etc. (2009-12-07).
	 ((and c-recognize-<>-arglists
	       (setq containing-< (c-up-list-backward indent-point containing-sexp))
	       (eq (char-after containing-<) ?\<))
	  (setq placeholder (c-point 'boi containing-<))
	  (goto-char containing-sexp)	; Most nested Lbrace/Lparen (but not
					; '<') before indent-point.
	  (if (>= (point) placeholder)
	      (progn
		(forward-char)
		(skip-chars-forward " \t"))
	    (goto-char placeholder))
	  (c-add-stmt-syntax 'template-args-cont (list containing-<) t
			     (c-most-enclosing-brace c-state-cache (point))
			     paren-state))
			     

	 ;; CASE 7: line is an expression, not a statement.  Most
	 ;; likely we are either in a function prototype or a function
	 ;; call argument list, or a template argument list.
	 ((not (or (and c-special-brace-lists
			(save-excursion
			  (goto-char containing-sexp)
			  (c-looking-at-special-brace-list)))
		   (eq (char-after containing-sexp) ?{)
		   (eq (char-after containing-sexp) ?<)))
	  (cond

	   ;; CASE 7A: we are looking at the arglist closing paren.
	   ;; C.f. case 7F.
	   ((memq char-after-ip '(?\) ?\]))
	    (goto-char containing-sexp)
	    (setq placeholder (c-point 'boi))
	    (if (and (c-safe (backward-up-list 1) t)
		     (>= (point) placeholder))
		(progn
		  (forward-char)
		  (skip-chars-forward " \t"))
	      (goto-char placeholder))
	    (c-add-stmt-syntax 'arglist-close (list containing-sexp) t
			       (c-most-enclosing-brace paren-state (point))
			       paren-state))

	   ;; CASE 7B: Looking at the opening brace of an
	   ;; in-expression block or brace list.  C.f. cases 4, 16A
	   ;; and 17E.
	   ((and (eq char-after-ip ?{)
		 (progn
		   (setq placeholder (c-inside-bracelist-p (point)
							   paren-state))
		   (if placeholder
		       (setq tmpsymbol '(brace-list-open . inexpr-class))
		     (setq tmpsymbol '(block-open . inexpr-statement)
			   placeholder
			   (cdr-safe (c-looking-at-inexpr-block
				      (c-safe-position containing-sexp
						       paren-state)
				      containing-sexp)))
		     ;; placeholder is nil if it's a block directly in
		     ;; a function arglist.  That makes us skip out of
		     ;; this case.
		     )))
	    (goto-char placeholder)
	    (back-to-indentation)
	    (c-add-stmt-syntax (car tmpsymbol) nil t
			       (c-most-enclosing-brace paren-state (point))
			       paren-state)
	    (if (/= (point) placeholder)
		(c-add-syntax (cdr tmpsymbol))))

	   ;; CASE 7C: we are looking at the first argument in an empty
	   ;; argument list. Use arglist-close if we're actually
	   ;; looking at a close paren or bracket.
	   ((memq char-before-ip '(?\( ?\[))
	    (goto-char containing-sexp)
	    (setq placeholder (c-point 'boi))
	    (if (and (c-safe (backward-up-list 1) t)
		     (>= (point) placeholder))
		(progn
		  (forward-char)
		  (skip-chars-forward " \t"))
	      (goto-char placeholder))
	    (c-add-stmt-syntax 'arglist-intro (list containing-sexp) t
			       (c-most-enclosing-brace paren-state (point))
			       paren-state))

	   ;; CASE 7D: we are inside a conditional test clause. treat
	   ;; these things as statements
	   ((progn
	      (goto-char containing-sexp)
	      (and (c-safe (c-forward-sexp -1) t)
		   (looking-at "\\<for\\>[^_]")))
	    (goto-char (1+ containing-sexp))
	    (c-forward-syntactic-ws indent-point)
	    (if (eq char-before-ip ?\;)
		(c-add-syntax 'statement (point))
	      (c-add-syntax 'statement-cont (point))
	      ))

	   ;; CASE 7E: maybe a continued ObjC method call. This is the
	   ;; case when we are inside a [] bracketed exp, and what
	   ;; precede the opening bracket is not an identifier.
	   ((and c-opt-method-key
		 (eq (char-after containing-sexp) ?\[)
		 (progn
		   (goto-char (1- containing-sexp))
		   (c-backward-syntactic-ws (c-point 'bod))
		   (if (not (looking-at c-symbol-key))
		       (c-add-syntax 'objc-method-call-cont containing-sexp))
		   )))

	   ;; CASE 7F: we are looking at an arglist continuation line,
	   ;; but the preceding argument is on the same line as the
	   ;; opening paren.  This case includes multi-line
	   ;; mathematical paren groupings, but we could be on a
	   ;; for-list continuation line.  C.f. case 7A.
	   ((progn
	      (goto-char (1+ containing-sexp))
	      (< (save-excursion
		   (c-forward-syntactic-ws)
		   (point))
		 (c-point 'bonl)))
	    (goto-char containing-sexp)	; paren opening the arglist
	    (setq placeholder (c-point 'boi))
	    (if (and (c-safe (backward-up-list 1) t)
		     (>= (point) placeholder))
		(progn
		  (forward-char)
		  (skip-chars-forward " \t"))
	      (goto-char placeholder))
	    (c-add-stmt-syntax 'arglist-cont-nonempty (list containing-sexp) t
			       (c-most-enclosing-brace c-state-cache (point))
			       paren-state))

	   ;; CASE 7G: we are looking at just a normal arglist
	   ;; continuation line
	   (t (c-forward-syntactic-ws indent-point)
	      (c-add-syntax 'arglist-cont (c-point 'boi)))
	   ))

	 ;; CASE 8: func-local multi-inheritance line
	 ((and (c-major-mode-is 'c++-mode)
	       (save-excursion
		 (goto-char indent-point)
		 (skip-chars-forward " \t")
		 (looking-at c-opt-postfix-decl-spec-key)))
	  (goto-char indent-point)
	  (skip-chars-forward " \t")
	  (cond

	   ;; CASE 8A: non-hanging colon on an inher intro
	   ((eq char-after-ip ?:)
	    (c-backward-syntactic-ws lim)
	    (c-add-syntax 'inher-intro (c-point 'boi)))

	   ;; CASE 8B: hanging colon on an inher intro
	   ((eq char-before-ip ?:)
	    (c-add-syntax 'inher-intro (c-point 'boi)))

	   ;; CASE 8C: a continued inheritance line
	   (t
	    (c-beginning-of-inheritance-list lim)
	    (c-add-syntax 'inher-cont (point))
	    )))

	 ;; CASE 9: we are inside a brace-list
	 ((and (not (c-major-mode-is 'awk-mode))  ; Maybe this isn't needed (ACM, 2002/3/29)
               (setq special-brace-list
                     (or (and c-special-brace-lists ;;;; ALWAYS NIL FOR AWK!!
                              (save-excursion
                                (goto-char containing-sexp)
                                (c-looking-at-special-brace-list)))
                         (c-inside-bracelist-p containing-sexp paren-state))))
	  (cond

	   ;; CASE 9A: In the middle of a special brace list opener.
	   ((and (consp special-brace-list)
		 (save-excursion
		   (goto-char containing-sexp)
		   (eq (char-after) ?\())
		 (eq char-after-ip (car (cdr special-brace-list))))
	    (goto-char (car (car special-brace-list)))
	    (skip-chars-backward " \t")
	    (if (and (bolp)
		     (assoc 'statement-cont
			    (setq placeholder (c-guess-basic-syntax))))
		(setq c-syntactic-context placeholder)
	      (c-beginning-of-statement-1
	       (c-safe-position (1- containing-sexp) paren-state))
	      (c-forward-token-2 0)
	      (while (looking-at c-specifier-key)
		(goto-char (match-end 1))
		(c-forward-syntactic-ws))
	      (c-add-syntax 'brace-list-open (c-point 'boi))))

	   ;; CASE 9B: brace-list-close brace
	   ((if (consp special-brace-list)
		;; Check special brace list closer.
		(progn
		  (goto-char (car (car special-brace-list)))
		  (save-excursion
		    (goto-char indent-point)
		    (back-to-indentation)
		    (or
		     ;; We were between the special close char and the `)'.
		     (and (eq (char-after) ?\))
			  (eq (1+ (point)) (cdr (car special-brace-list))))
		     ;; We were before the special close char.
		     (and (eq (char-after) (cdr (cdr special-brace-list)))
			  (zerop (c-forward-token-2))
			  (eq (1+ (point)) (cdr (car special-brace-list)))))))
	      ;; Normal brace list check.
	      (and (eq char-after-ip ?})
		   (c-safe (goto-char (c-up-list-backward (point))) t)
		   (= (point) containing-sexp)))
	    (if (eq (point) (c-point 'boi))
		(c-add-syntax 'brace-list-close (point))
	      (setq lim (c-most-enclosing-brace c-state-cache (point)))
	      (c-beginning-of-statement-1 lim)
	      (c-add-stmt-syntax 'brace-list-close nil t lim paren-state)))

	   (t
	    ;; Prepare for the rest of the cases below by going to the
	    ;; token following the opening brace
	    (if (consp special-brace-list)
		(progn
		  (goto-char (car (car special-brace-list)))
		  (c-forward-token-2 1 nil indent-point))
	      (goto-char containing-sexp))
	    (forward-char)
	    (let ((start (point)))
	      (c-forward-syntactic-ws indent-point)
	      (goto-char (max start (c-point 'bol))))
	    (c-skip-ws-forward indent-point)
	    (cond

	     ;; CASE 9C: we're looking at the first line in a brace-list
	     ((= (point) indent-point)
	      (if (consp special-brace-list)
		  (goto-char (car (car special-brace-list)))
		(goto-char containing-sexp))
	      (if (eq (point) (c-point 'boi))
		  (c-add-syntax 'brace-list-intro (point))
		(setq lim (c-most-enclosing-brace c-state-cache (point)))
		(c-beginning-of-statement-1 lim)
		(c-add-stmt-syntax 'brace-list-intro nil t lim paren-state)))

	     ;; CASE 9D: this is just a later brace-list-entry or
	     ;; brace-entry-open
	     (t (if (or (eq char-after-ip ?{)
			(and c-special-brace-lists
			     (save-excursion
			       (goto-char indent-point)
			       (c-forward-syntactic-ws (c-point 'eol))
			       (c-looking-at-special-brace-list (point)))))
		    (c-add-syntax 'brace-entry-open (point))
		  (c-add-syntax 'brace-list-entry (point))
		  ))
	     ))))

	 ;; CASE 10: A continued statement or top level construct.
	 ((and (not (memq char-before-ip '(?\; ?:)))
	       (not (c-at-vsemi-p before-ws-ip))
	       (or (not (eq char-before-ip ?}))
		   (c-looking-at-inexpr-block-backward c-state-cache))
	       (> (point)
		  (save-excursion
		    (c-beginning-of-statement-1 containing-sexp)
		    (setq placeholder (point))))
	       (/= placeholder containing-sexp))
	  ;; This is shared with case 18.
	  (c-guess-continued-construct indent-point
				       char-after-ip
				       placeholder
				       containing-sexp
				       paren-state))

	 ;; CASE 16: block close brace, possibly closing the defun or
	 ;; the class
	 ((eq char-after-ip ?})
	  ;; From here on we have the next containing sexp in lim.
	  (setq lim (c-most-enclosing-brace paren-state))
	  (goto-char containing-sexp)
	    (cond

	     ;; CASE 16E: Closing a statement block?  This catches
	     ;; cases where it's preceded by a statement keyword,
	     ;; which works even when used in an "invalid" context,
	     ;; e.g. a macro argument.
	     ((c-after-conditional)
	      (c-backward-to-block-anchor lim)
	      (c-add-stmt-syntax 'block-close nil t lim paren-state))

	     ;; CASE 16A: closing a lambda defun or an in-expression
	     ;; block?  C.f. cases 4, 7B and 17E.
	     ((setq placeholder (c-looking-at-inexpr-block
				 (c-safe-position containing-sexp paren-state)
				 nil))
	      (setq tmpsymbol (if (eq (car placeholder) 'inlambda)
				  'inline-close
				'block-close))
	      (goto-char containing-sexp)
	      (back-to-indentation)
	      (if (= containing-sexp (point))
		  (c-add-syntax tmpsymbol (point))
		(goto-char (cdr placeholder))
		(back-to-indentation)
		(c-add-stmt-syntax tmpsymbol nil t
				   (c-most-enclosing-brace paren-state (point))
				   paren-state)
		(if (/= (point) (cdr placeholder))
		    (c-add-syntax (car placeholder)))))

	     ;; CASE 16B: does this close an inline or a function in
	     ;; a non-class declaration level block?
	     ((save-excursion
		(and lim
		     (progn
		       (goto-char lim)
		       (c-looking-at-decl-block
			(c-most-enclosing-brace paren-state lim)
			nil))
		     (setq placeholder (point))))
	      (c-backward-to-decl-anchor lim)
	      (back-to-indentation)
	      (if (save-excursion
		    (goto-char placeholder)
		    (looking-at c-other-decl-block-key))
		  (c-add-syntax 'defun-close (point))
		(c-add-syntax 'inline-close (point))))

	     ;; CASE 16F: Can be a defun-close of a function declared
	     ;; in a statement block, e.g. in Pike or when using gcc
	     ;; extensions, but watch out for macros followed by
	     ;; blocks.  Let it through to be handled below.
	     ;; C.f. cases B.3 and 17G.
	     ((save-excursion
		(and (not (c-at-statement-start-p))
		     (eq (c-beginning-of-statement-1 lim nil nil t) 'same)
		     (setq placeholder (point))
		     (let ((c-recognize-typeless-decls nil))
		       ;; Turn off recognition of constructs that
		       ;; lacks a type in this case, since that's more
		       ;; likely to be a macro followed by a block.
		       (c-forward-decl-or-cast-1 (c-point 'bosws) nil nil))))
	      (back-to-indentation)
	      (if (/= (point) containing-sexp)
		  (goto-char placeholder))
	      (c-add-stmt-syntax 'defun-close nil t lim paren-state))

	     ;; CASE 16C: If there is an enclosing brace then this is
	     ;; a block close since defun closes inside declaration
	     ;; level blocks have been handled above.
	     (lim
	      ;; If the block is preceded by a case/switch label on
	      ;; the same line, we anchor at the first preceding label
	      ;; at boi.  The default handling in c-add-stmt-syntax
	      ;; really fixes it better, but we do like this to keep
	      ;; the indentation compatible with version 5.28 and
	      ;; earlier.  C.f. case 17H.
	      (while (and (/= (setq placeholder (point)) (c-point 'boi))
			  (eq (c-beginning-of-statement-1 lim) 'label)))
	      (goto-char placeholder)
	      (if (looking-at c-label-kwds-regexp)
		  (c-add-syntax 'block-close (point))
		(goto-char containing-sexp)
		;; c-backward-to-block-anchor not necessary here; those
		;; situations are handled in case 16E above.
		(c-add-stmt-syntax 'block-close nil t lim paren-state)))

	     ;; CASE 16D: Only top level defun close left.
	     (t
	      (goto-char containing-sexp)
	      (c-backward-to-decl-anchor lim)
	      (c-add-stmt-syntax 'defun-close nil nil
				 (c-most-enclosing-brace paren-state)
				 paren-state))
	     ))

	 ;; CASE 17: Statement or defun catchall.
	 (t
	  (goto-char indent-point)
	  ;; Back up statements until we find one that starts at boi.
	  (while (let* ((prev-point (point))
			(last-step-type (c-beginning-of-statement-1
					 containing-sexp)))
		   (if (= (point) prev-point)
		       (progn
			 (setq step-type (or step-type last-step-type))
			 nil)
		     (setq step-type last-step-type)
		     (/= (point) (c-point 'boi)))))
	  (cond

	   ;; CASE 17B: continued statement
	   ((and (eq step-type 'same)
		 (/= (point) indent-point))
	    (c-add-stmt-syntax 'statement-cont nil nil
			       containing-sexp paren-state))

	   ;; CASE 17A: After a case/default label?
	   ((progn
	      (while (and (eq step-type 'label)
			  (not (looking-at c-label-kwds-regexp)))
		(setq step-type
		      (c-beginning-of-statement-1 containing-sexp)))
	      (eq step-type 'label))
	    (c-add-stmt-syntax (if (eq char-after-ip ?{)
				   'statement-case-open
				 'statement-case-intro)
			       nil t containing-sexp paren-state))

	   ;; CASE 17D: any old statement
	   ((progn
	      (while (eq step-type 'label)
		(setq step-type
		      (c-beginning-of-statement-1 containing-sexp)))
	      (eq step-type 'previous))
	    (c-add-stmt-syntax 'statement nil t
			       containing-sexp paren-state)
	    (if (eq char-after-ip ?{)
		(c-add-syntax 'block-open)))

	   ;; CASE 17I: Inside a substatement block.
	   ((progn
	      ;; The following tests are all based on containing-sexp.
	      (goto-char containing-sexp)
	      ;; From here on we have the next containing sexp in lim.
	      (setq lim (c-most-enclosing-brace paren-state containing-sexp))
	      (c-after-conditional))
	    (c-backward-to-block-anchor lim)
	    (c-add-stmt-syntax 'statement-block-intro nil t
			       lim paren-state)
	    (if (eq char-after-ip ?{)
		(c-add-syntax 'block-open)))

	   ;; CASE 17E: first statement in an in-expression block.
	   ;; C.f. cases 4, 7B and 16A.
	   ((setq placeholder (c-looking-at-inexpr-block
			       (c-safe-position containing-sexp paren-state)
			       nil))
	    (setq tmpsymbol (if (eq (car placeholder) 'inlambda)
				'defun-block-intro
			      'statement-block-intro))
	    (back-to-indentation)
	    (if (= containing-sexp (point))
		(c-add-syntax tmpsymbol (point))
	      (goto-char (cdr placeholder))
	      (back-to-indentation)
	      (c-add-stmt-syntax tmpsymbol nil t
				 (c-most-enclosing-brace c-state-cache (point))
				 paren-state)
	      (if (/= (point) (cdr placeholder))
		  (c-add-syntax (car placeholder))))
	    (if (eq char-after-ip ?{)
		(c-add-syntax 'block-open)))

	   ;; CASE 17F: first statement in an inline, or first
	   ;; statement in a top-level defun. we can tell this is it
	   ;; if there are no enclosing braces that haven't been
	   ;; narrowed out by a class (i.e. don't use bod here).
	   ((save-excursion
	      (or (not (setq placeholder (c-most-enclosing-brace
					  paren-state)))
		  (and (progn
			 (goto-char placeholder)
			 (eq (char-after) ?{))
		       (c-looking-at-decl-block (c-most-enclosing-brace
						 paren-state (point))
						nil))))
	    (c-backward-to-decl-anchor lim)
	    (back-to-indentation)
	    (c-add-syntax 'defun-block-intro (point)))

	   ;; CASE 17G: First statement in a function declared inside
	   ;; a normal block.  This can occur in Pike and with
	   ;; e.g. the gcc extensions, but watch out for macros
	   ;; followed by blocks.  C.f. cases B.3 and 16F.
	   ((save-excursion
	      (and (not (c-at-statement-start-p))
		   (eq (c-beginning-of-statement-1 lim nil nil t) 'same)
		   (setq placeholder (point))
		   (let ((c-recognize-typeless-decls nil))
		     ;; Turn off recognition of constructs that lacks
		     ;; a type in this case, since that's more likely
		     ;; to be a macro followed by a block.
		     (c-forward-decl-or-cast-1 (c-point 'bosws) nil nil))))
	    (back-to-indentation)
	    (if (/= (point) containing-sexp)
		(goto-char placeholder))
	    (c-add-stmt-syntax 'defun-block-intro nil t
			       lim paren-state))

	   ;; CASE 17H: First statement in a block.
	   (t
	    ;; If the block is preceded by a case/switch label on the
	    ;; same line, we anchor at the first preceding label at
	    ;; boi.  The default handling in c-add-stmt-syntax is
	    ;; really fixes it better, but we do like this to keep the
	    ;; indentation compatible with version 5.28 and earlier.
	    ;; C.f. case 16C.
	    (while (and (/= (setq placeholder (point)) (c-point 'boi))
			(eq (c-beginning-of-statement-1 lim) 'label)))
	    (goto-char placeholder)
	    (if (looking-at c-label-kwds-regexp)
		(c-add-syntax 'statement-block-intro (point))
	      (goto-char containing-sexp)
	      ;; c-backward-to-block-anchor not necessary here; those
	      ;; situations are handled in case 17I above.
	      (c-add-stmt-syntax 'statement-block-intro nil t
				 lim paren-state))
	    (if (eq char-after-ip ?{)
		(c-add-syntax 'block-open)))
	   ))
	 )

	;; now we need to look at any modifiers
	(goto-char indent-point)
	(skip-chars-forward " \t")

	;; are we looking at a comment only line?
	(when (and (looking-at c-comment-start-regexp)
		   (/= (c-forward-token-2 0 nil (c-point 'eol)) 0))
	  (c-append-syntax 'comment-intro))

	;; we might want to give additional offset to friends (in C++).
	(when (and c-opt-friend-key
		   (looking-at c-opt-friend-key))
	  (c-append-syntax 'friend))

	;; Set syntactic-relpos.
	(let ((p c-syntactic-context))
	  (while (and p
		      (if (integerp (c-langelem-pos (car p)))
			  (progn
			    (setq syntactic-relpos (c-langelem-pos (car p)))
			    nil)
			t))
	    (setq p (cdr p))))

	;; Start of or a continuation of a preprocessor directive?
	(if (and macro-start
		 (eq macro-start (c-point 'boi))
		 (not (and (c-major-mode-is 'pike-mode)
			   (eq (char-after (1+ macro-start)) ?\"))))
	    (c-append-syntax 'cpp-macro)
	  (when (and c-syntactic-indentation-in-macros macro-start)
	    (if in-macro-expr
		(when (or
		       (< syntactic-relpos macro-start)
		       (not (or
			     (assq 'arglist-intro c-syntactic-context)
			     (assq 'arglist-cont c-syntactic-context)
			     (assq 'arglist-cont-nonempty c-syntactic-context)
			     (assq 'arglist-close c-syntactic-context))))
		  ;; If inside a cpp expression, i.e. anywhere in a
		  ;; cpp directive except a #define body, we only let
		  ;; through the syntactic analysis that is internal
		  ;; in the expression.  That means the arglist
		  ;; elements, if they are anchored inside the cpp
		  ;; expression.
		  (setq c-syntactic-context nil)
		  (c-add-syntax 'cpp-macro-cont macro-start))
	      (when (and (eq macro-start syntactic-relpos)
			 (not (assq 'cpp-define-intro c-syntactic-context))
			 (save-excursion
			   (goto-char macro-start)
			   (or (not (c-forward-to-cpp-define-body))
			       (<= (point) (c-point 'boi indent-point)))))
		;; Inside a #define body and the syntactic analysis is
		;; anchored on the start of the #define.  In this case
		;; we add cpp-define-intro to get the extra
		;; indentation of the #define body.
		(c-add-syntax 'cpp-define-intro)))))

	;; return the syntax
	c-syntactic-context)))

;;;; End of `k0-c-guess-basic-syntax'

(provide 'k0-mode)
