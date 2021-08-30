BUNDLE = lv2pftci-synthFold00MUS.lv2
INSTALL_DIR = /usr/local/lib/lv2


$(BUNDLE): manifest.ttl synthFold00MUS.ttl synthFold00MUS.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp $^ $(BUNDLE)

synthFold00MUS.so: synthFold00MUS.cpp 
	g++ -shared -fPIC -DPIC synthFold00MUS.cpp `pkg-config --cflags --libs lv2-plugin` -o synthFold00MUS.so


install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

clean:
	rm -rf $(BUNDLE) synthFold00MUS.so 
