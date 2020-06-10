(define-key global-map "\C-l" 'goto-line)                                     
(define-key global-map "\em" 'compile)                                        
(define-key global-map [(ctrl home)]'beginning-of-buffer)                     
(define-key global-map [(ctrl end)]'end-of-buffer)                            
(define-key global-map [(shift end)]'other-window)                            
(define-key global-map [(shift down)] 'next-error)                            
(define-key global-map [(shift f1)] 'call-last-kbd-macro)                     
(global-set-key "\C-h" 'backward-delete-char)                                 
                                                                              
(set-background-color "black");                                               
(set-foreground-color "cyan");                                                
(global-font-lock-mode t)                                                     
                                                                              
                                                                              
                                                          