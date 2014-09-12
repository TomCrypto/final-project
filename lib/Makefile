DEPS             = freeimage fftw anttweakbar glew
OUTDIR           = bin/linux
BUILDDIR         = .build

all: $(DEPS)

freeimage: $(OUTDIR)/libfreeimage.a

$(OUTDIR)/libfreeimage.a: $(BUILDDIR)/FreeImage/Dist/libfreeimage.a
	cp $^ $@

$(BUILDDIR)/FreeImage/Dist/libfreeimage.a: $(BUILDDIR)/FreeImage/Makefile
	cd $(BUILDDIR)/FreeImage && make -j 2

$(BUILDDIR)/FreeImage/Makefile: $(BUILDDIR)/freeimage.zip
	rm -rf $(BUILDDIR)/FreeImage
	unzip $< -d $(BUILDDIR)
	touch $@

$(BUILDDIR)/freeimage.zip:
	wget http://softlayer-dal.dl.sourceforge.net/project/freeimage/Source%20Distribution/3.16.0/FreeImage3160.zip -O $@

fftw: $(OUTDIR)/libfftw3f.a $(OUTDIR)/libfftw3f_omp.a

$(OUTDIR)/libfftw3f.a: $(BUILDDIR)/fftw-3.3.4/.libs/libfftw3f.a
	cp $^ $@

$(OUTDIR)/libfftw3f_omp.a: $(BUILDDIR)/fftw-3.3.4/threads/.libs/libfftw3f_omp.a
	cp $^ $@

$(BUILDDIR)/fftw-3.3.4/.libs/libfftw3f.a: $(BUILDDIR)/fftw-3.3.4/Makefile
	cd $(BUILDDIR)/fftw-3.3.4 && make -j 2

$(BUILDDIR)/fftw-3.3.4/threads/.libs/libfftw3f_omp.a: $(BUILDDIR)/fftw-3.3.4/Makefile
	cd $(BUILDDIR)/fftw-3.3.4 && make -j 2

$(BUILDDIR)/fftw-3.3.4/Makefile: $(BUILDDIR)/fftw-3.3.4/configure
	cd $(BUILDDIR)/fftw-3.3.4 && ./configure --enable-openmp --enable-float

$(BUILDDIR)/fftw-3.3.4/configure: $(BUILDDIR)/fftw3.tar.gz
	tar -zxvf $< -C $(BUILDDIR)
	touch $@

$(BUILDDIR)/fftw3.tar.gz:
	wget http://www.fftw.org/fftw-3.3.4.tar.gz -O $@

anttweakbar: $(OUTDIR)/libAntTweakBar.a

$(OUTDIR)/libAntTweakBar.a: $(BUILDDIR)/AntTweakBar/lib/libAntTweakBar.a
	cp $^ $@

$(BUILDDIR)/AntTweakBar/lib/libAntTweakBar.a: $(BUILDDIR)/AntTweakBar/src/Makefile
	cd $(BUILDDIR)/AntTweakBar/src && make -j 2

$(BUILDDIR)/AntTweakBar/src/Makefile: $(BUILDDIR)/anttweakbar.zip
	rm -rf $(BUILDDIR)/AntTweakBar
	unzip $< -d $(BUILDDIR)
	touch $@

$(BUILDDIR)/anttweakbar.zip:
	wget http://hivelocity.dl.sourceforge.net/project/anttweakbar/AntTweakBar_116.zip -O $@

glew: $(OUTDIR)/libGLEW.a

$(OUTDIR)/libGLEW.a: $(BUILDDIR)/glew-master/lib/libGLEW.a
	cp $^ $@

$(BUILDDIR)/glew-master/lib/libGLEW.a: $(BUILDDIR)/glew-master/Makefile
	cd $(BUILDDIR)/glew-master/auto && make -j 2 && cd .. && make -j 2

$(BUILDDIR)/glew-master/Makefile: $(BUILDDIR)/glew.zip
	rm -rf $(BUILDDIR)/glew-master
	unzip $< -d $(BUILDDIR)
	touch $@

$(BUILDDIR)/glew.zip:
	wget https://github.com/nigels-com/glew/archive/master.zip -O $@

clean:
	rm -rf $(BUILDDIR)/* $(OUTDIR)/libfreeimage.a $(OUTDIR)/libfftw3f.a $(OUTDIR)/libfftw3f_omp.a $(OUTDIR)/libAntTweakBar.a $(OUTDIR)/libGLEW.a

.PHONY: all $(DEPS) clean
