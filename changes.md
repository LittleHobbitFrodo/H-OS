
## All changes since last push:

- [x] 2mb pages are working
	- virtual_address structure is now (only) union
	- page entries are now sized and unsized
	- added any_page_entry union
	- page_bits enum is being deprecated
    - quick mapping stopped working (not fixing it (now))


- [ ] changes in page heap scheme:
	- each allocator holds 512 tables
	- the allocator structure is huge
	- page heap is renamed to table heap (makes sense)

### misc.
- [x] refactoring of deprecated mnemonic suffix with in and out instructions