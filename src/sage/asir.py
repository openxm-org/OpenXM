# $OpenXM: OpenXM/src/sage/asir.py,v 1.2 2019/03/06 02:38:33 takayama Exp $
import os
from sage.interfaces.expect import Expect, ExpectElement
from sage.misc.verbose import verbose
from sage.cpython.string import bytes_to_str

##Ref: @s/2018/09/20180907-sage-asir-proj, Using External Libraries and Interfaces
##Ref: /usr/lib/python2.7/dist-packages/sage/interfaces
##Usage: load("asir.py");


class Asir(Expect):
    r"""
    Interface to the Asir interpreter.

    EXAMPLES::

        sage: asir.evall("F=fctr(x^10-1)")    # optional - asir
    """
    def __init__(self, maxread=None, script_subdirectory=None, logfile=None,
                 server=None, server_tmpdir=None, seed=None, command=None):
        """
        EXAMPLES::

            sage: asir == loads(dumps(asir))
            True
        """
        if command is None:
            command = os.getenv('SAGE_ASIR_COMMAND') or 'openxm ox_texmacs --view sage --quiet --noCopyright'
##Other options are --noLogWindow and --outputLimit size(in bytes)
        if server is None:
            server = os.getenv('SAGE_ASIR_SERVER') or None
        Expect.__init__(self,
                        name='asir',
                        # We want the prompt sequence to be unique to avoid confusion with syntax error messages containing >>>
                        prompt='asir>',
                        # We don't want any pagination of output
                        command='openxm ox_texmacs --view sage --quiet --noCopyright',
                        maxread=maxread,
                        server=server,
                        server_tmpdir=server_tmpdir,
                        script_subdirectory=script_subdirectory,
                        restart_on_ctrlc=False,
                        verbose_start=False,
                        logfile=logfile,
                        eval_using_file_cutoff=100)
        self._seed = seed

    def set_seed(self, seed=None):
        """
        Not implemented. Set the seed for the random number generator
        for this asir interpreter.
        """
        return 0

    def __reduce__(self):
        """
        EXAMPLES::

            sage: asir.__reduce__()
            (<function reduce_load_Asir at 0x...>, ())
        """
        return reduce_load_Asir, tuple()

    def _read_in_file_command(self, filename):
        """
        EXAMPLES::

            sage: filename = tmp_filename()
            sage: asir._read_in_file_command(filename)
            'load("...");'
        """
        return 'load("%s");' % filename

    def _quit_string(self):
        """
        EXAMPLES::

            sage: asir._quit_string()
            'quit;'
        """
        return '!quit;'

    def _install_hints(self):
        """
        Return hints on how to install Asir.

        EXAMPLES::

            sage: print(asir._install_hints())
            You must get ...
        """
        return """
        You must get the program "asir" and "ox_texmacs" in order to use Asir
        from Sage. You can read all about Asir at
                http://www.openxm.org
        The command "openxm" must be in the search path.
        """

    def evall(self, cmd):
        """
        Evaluate the argument immediately. The argument of eval is buffered
        and ; should be added.

        EXAMPLES::

            sage: asir.eval('1+2;'); asir.evall('3+3')
        """
        return self.eval(cmd+'; ;')

    def _eval_line(self, line, reformat=True, allow_use_file=False,
                   wait_for_prompt=True, restart_if_needed=False):
        """
        EXAMPLES::

            sage: print(asir._eval_line('2+2'))  #optional - asir
            '4'
        """
        from pexpect.exceptions import EOF
        if not wait_for_prompt:
            return Expect._eval_line(self, line)
        if line == '':
            return ''
        if self._expect is None:
            self._start()
        if allow_use_file and len(line) > 3000:
            return self._eval_line_using_file(line)
        try:
            E = self._expect
            # debug
            # self._synchronize(cmd='1+%s\n')
            verbose("in = '%s'" % line, level=3)
            E.sendline(line)
            E.expect(self._prompt)
            out = bytes_to_str(E.before)
            # debug
            verbose("out = '%s'" % out, level=3)
        except EOF:
            if self._quit_string() in line:
                return ''
        except KeyboardInterrupt:
            self._keyboard_interrupt()
        try:
            if reformat:
                if 'syntax error' in out:
                    raise SyntaxError(out)
            out = "\n".join(out.splitlines()[1:])
            return out
        except NameError:
            return ''

    def _keyboard_interrupt(self):
        print("Ctrl-C: Interrupting %s..." % self)
        if self._restart_on_ctrlc:
            try:
                self._expect.close(force=1)
            except pexpect.ExceptionPexpect as msg:
                raise RuntimeError("THIS IS A BUG -- PLEASE REPORT. This should never happen.\n" + msg)
            self._start()
            raise KeyboardInterrupt("Restarting %s (WARNING: all variables defined in previous session are now invalid)"%self)
        else:
            self._expect.send('\003') # control-c
            raise KeyboardInterrupt("Ctrl-c pressed while running %s"%self)

    def quit(self, verbose=False):
        """
        EXAMPLES::

            sage: o = Asir()
            sage: o._start()    # optional - asir
            sage: o.quit(True)  # optional - asir
            Exiting spawned Asir process.
        """
        # Don't bother, since it just hangs in some cases, and it
        # isn't necessary, since asir behaves well with respect
        # to signals.
        if not self._expect is None:
            if verbose:
                print("Exiting spawned %s process." % self)
        return

    def _start(self):
        """
        Start the Asir process.

        EXAMPLES::

            sage: o = Asir()    # optional - asir
            sage: o.is_running()  # optional - asir
            False
            sage: o._start()      # optional - asir
            sage: o.is_running()  # optional - asir
            True
        """
        Expect._start(self)
#        self.eval("page_screen_output=0;")
#        self.eval("format none;")
        # set random seed
#        self.set_seed(self._seed)

    def _equality_symbol(self):
        """
        EXAMPLES::

            sage: asir('0 == 1')  # optional - asir
            0
            sage: asir('1 == 1')  # optional - asir
            1
        """
        return '=='

    def _true_symbol(self):
        """
        EXAMPLES::

            sage: asir('1 == 1')  # optional - asir
            1
        """
        return '1'

    def _false_symbol(self):
        """
        EXAMPLES::

            sage: asir('0 == 1')  # optional - asir
            0
        """
        return '0'

    def set(self, var, value):
        """
        Set the variable ``var`` to the given ``value``.

        EXAMPLES::

            sage: asir.set('X', '2') # optional - asir
            sage: asir.get('X') # optional - asir
            ' 2'
        """
        cmd = '%s=%s' % (var, value)
        out = self.evall(cmd)
        if out.find("error") != -1 or out.find("Error") != -1:
            raise TypeError("Error executing code in Asir\nCODE:\n\t%s\nAsir ERROR:\n\t%s" % (cmd, out))

    def get(self, var):
        """
        Get the value of the variable ``var``.

        EXAMPLES::

            sage: asir.set('X', '2') # optional - asir
            sage: asir.get('X') # optional - asir
            ' 2'
        """
        s = self.evall('%s;' % var)
        i = s.find('=')
        return s[i+1:]

    def console(self):
        """
        Spawn a new Asir command-line session.

        This requires that the optional ``asir`` program be installed and in
        your ``PATH``, but no optional Sage packages need be installed.

        EXAMPLES::

            sage: asir_console()  # not tested
            This is Risa/Asir, ....
            ...
            [...] 2+3;
            5
            [...] quit();

        The command ``quit();`` exits the ``asir`` console and returns you to Sage.
        """
        asir_console()

    def version(self) -> str:
        """
        Return the version of Asir.

        bug: it returns error because sage tries to set sage0=version();
             instead of Sage0=version();
        OUTPUT: string

        EXAMPLES::

            sage: v = asir.version()   # optional - asir
            sage: v                      # optional - asir; random
            '2.13.7'
        """
        return str(self("version()")).strip()

    def _object_class(self):
        """
        EXAMPLES::

            sage: asir._object_class()
            <class 'sage.interfaces.asir.AsirElement'>
        """
        return AsirElement


asir_functions = set()

#Todo, the following class has not yet been implemented.
#      These are sample interface with octave.
class AsirElement(ExpectElement):
    def _get_sage_ring(self):
        r"""
        TESTS::

            sage: asir('1')._get_sage_ring()  # optional - asir
            Real Double Field
            sage: asir('I')._get_sage_ring()  # optional - asir
            Complex Double Field
            sage: asir('[]')._get_sage_ring() # optional - asir
            Real Double Field
        """
        if self.isinteger():
            import sage.rings.integer_ring
            return sage.rings.integer_ring.ZZ
        elif self.isreal():
            import sage.rings.real_double
            return sage.rings.real_double.RDF
        elif self.iscomplex():
            import sage.rings.complex_double
            return sage.rings.complex_double.CDF
        else:
            raise TypeError("no Sage ring associated to this element.")

    def __nonzero__(self):
        r"""
        Test whether this element is nonzero.

        EXAMPLES::

            sage: bool(asir('0'))                 # optional - asir
            False
            sage: bool(asir('[]'))                # optional - asir
            False
            sage: bool(asir('[0,0]'))             # optional - asir
            False
            sage: bool(asir('[0,0,0;0,0,0]'))     # optional - asir
            False

            sage: bool(asir('0.1'))               # optional - asir
            True
            sage: bool(asir('[0,1,0]'))           # optional - asir
            True
            sage: bool(asir('[0,0,-0.1;0,0,0]'))  # optional - asir
            True
        """
        return str(self) != ' [](0x0)' and any(x != '0' for x in str(self).split())

    def _matrix_(self, R=None):
        r"""
        Return Sage matrix from this ``asir`` element.

        EXAMPLES::

            sage: A = asir('[1,2;3,4.5]')     # optional - asir
            sage: matrix(A)                     # optional - asir
            [1.0 2.0]
            [3.0 4.5]
            sage: _.base_ring()                 # optional - asir
            Real Double Field

            sage: A = asir('[I,1;-1,0]')      # optional - asir
            sage: matrix(A)                     # optional - asir
            [1.0*I   1.0]
            [ -1.0   0.0]
            sage: _.base_ring()                 # optional - asir
            Complex Double Field

            sage: A = asir('[1,2;3,4]')       # optional - asir
            sage: matrix(ZZ, A)                 # optional - asir
            [1 2]
            [3 4]
            sage: A = asir('[1,2;3,4.5]')     # optional - asir
            sage: matrix(RR, A)                 # optional - asir
            [1.00000000000000 2.00000000000000]
            [3.00000000000000 4.50000000000000]
        """
        if not self.ismatrix():
            raise TypeError('not an asir matrix')
        if R is None:
            R = self._get_sage_ring()

        s = str(self).strip('\n ')
        w = [u.strip().split(' ') for u in s.split('\n')]
        nrows = len(w)
        ncols = len(w[0])

        if self.iscomplex():
            w = [[to_complex(x,R) for x in row] for row in w]

        from sage.matrix.all import MatrixSpace
        return MatrixSpace(R, nrows, ncols)(w)

    def _vector_(self, R=None):
        r"""
        Return Sage vector from this asir element.

        EXAMPLES::

            sage: A = asir('[1,2,3,4]')       # optional - asir
            sage: vector(ZZ, A)                 # optional - asir
            (1, 2, 3, 4)
            sage: A = asir('[1,2.3,4.5]')     # optional - asir
            sage: vector(A)                     # optional - asir
            (1.0, 2.3, 4.5)
            sage: A = asir('[1,I]')           # optional - asir
            sage: vector(A)                     # optional - asir
            (1.0, 1.0*I)
        """
        oc = self.parent()
        if not self.isvector():
            raise TypeError('not an asir vector')
        if R is None:
            R = self._get_sage_ring()

        s = str(self).strip('\n ')
        w = s.strip().split(' ')
        nrows = len(w)

        if self.iscomplex():
            w = [to_complex(x, R) for x in w]

        from sage.modules.free_module import FreeModule
        return FreeModule(R, nrows)(w)

    def _scalar_(self):
        """
        Return Sage scalar from this asir element.

        EXAMPLES::

            sage: A = asir('2833')      # optional - asir
            sage: As = A.sage(); As       # optional - asir
            2833.0
            sage: As.parent()             # optional - asir
            Real Double Field

            sage: B = sqrt(A)             # optional - asir
            sage: Bs = B.sage(); Bs       # optional - asir
            53.2259
            sage: Bs.parent()             # optional - asir
            Real Double Field

            sage: C = sqrt(-A)            # optional - asir
            sage: Cs = C.sage(); Cs       # optional - asir
            53.2259*I
            sage: Cs.parent()             # optional - asir
            Complex Double Field
        """
        if not self.isscalar():
            raise TypeError("not an asir scalar")

        R = self._get_sage_ring()
        if self.iscomplex():
            return to_complex(str(self), R)
        else:
            return R(str(self))

    def _sage_(self):
        """
        Try to parse the asir object and return a sage object.

        EXAMPLES::

            sage: A = asir('2833')           # optional - asir
            sage: A.sage()                     # optional - asir
            2833.0
            sage: B = sqrt(A)                  # optional - asir
            sage: B.sage()                     # optional - asir
            53.2259
            sage: C = sqrt(-A)                 # optional - asir
            sage: C.sage()                     # optional - asir
            53.2259*I
            sage: A = asir('[1,2,3,4]')      # optional - asir
            sage: A.sage()                     # optional - asir
            (1.0, 2.0, 3.0, 4.0)
            sage: A = asir('[1,2.3,4.5]')    # optional - asir
            sage: A.sage()                     # optional - asir
            (1.0, 2.3, 4.5)
            sage: A = asir('[1,2.3+I,4.5]')  # optional - asir
            sage: A.sage()                     # optional - asir
            (1.0, 2.3 + 1.0*I, 4.5)
        """
        if self.isscalar():
            return self._scalar_()
        if self.isvector():
            return self._vector_()
        if self.ismatrix():
            return self._matrix_()
        raise NotImplementedError('asir type is not recognized')

# An instance
asir = Asir()

def reduce_load_Asir():
    """
    EXAMPLES::

        sage: from sage.interfaces.asir import reduce_load_Asir
        sage: reduce_load_Asir()
        Asir
    """
    return asir


def asir_console():
    """
    Spawn a new Asir command-line session.

    This requires that the optional ``asir`` program be installed and in
    your ``PATH``, but no optional Sage packages need be installed.

    EXAMPLES::

        sage: asir_console()         # not tested
        This is Risa/Asir ....
        ...
        [nnnn] 2+3;
        5
        [nnnn] quit();

    quit(); exits the asir console and returns you to Sage.
    """
    from sage.repl.rich_output.display_manager import get_display_manager
    if not get_display_manager().is_in_terminal():
        raise RuntimeError('Can use the console only in the terminal. Try %%asir magics instead.')
    os.system('openxm asir')    # with asir prompt
#    os.system('openxm asir -quiet')


