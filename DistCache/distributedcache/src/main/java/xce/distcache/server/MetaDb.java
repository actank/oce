package xce.distcache.server;

import java.io.File;
import java.util.HashSet;
import java.util.concurrent.TimeUnit;

import com.sleepycat.bind.EntryBinding;
import com.sleepycat.bind.serial.SerialBinding;
import com.sleepycat.bind.serial.StoredClassCatalog;
import com.sleepycat.bind.tuple.TupleBinding;
import com.sleepycat.je.Database;
import com.sleepycat.je.DatabaseConfig;
import com.sleepycat.je.DatabaseEntry;
import com.sleepycat.je.Environment;
import com.sleepycat.je.EnvironmentConfig;
import com.sleepycat.je.LockMode;
import com.sleepycat.je.OperationStatus;
import com.sleepycat.je.Transaction;

public class MetaDb {
    private Environment _env;
    private Database _db;
    private StoredClassCatalog classCatalog;
    @SuppressWarnings("unchecked")
    private EntryBinding<HashSet> valueBinding;
    private EntryBinding<Integer> keyBinding;

    protected MetaDb() {
    }

    @SuppressWarnings("unchecked")
    protected void initialize(String dataDir) {
	EnvironmentConfig envConfig = new EnvironmentConfig();
	envConfig.setAllowCreate(true);
	envConfig.setTransactional(true);
	envConfig.setTxnTimeout(1, TimeUnit.SECONDS);
	_env = new Environment(new File(dataDir), envConfig);

	DatabaseConfig dbConfig = new DatabaseConfig();
	dbConfig.setAllowCreate(true);
	dbConfig.setTransactional(true);
	_db = _env.openDatabase(null, "META", dbConfig);

	classCatalog = new StoredClassCatalog(_db);
	valueBinding = new SerialBinding<HashSet>(classCatalog, HashSet.class);
	keyBinding = TupleBinding.getPrimitiveBinding(Integer.class);
    }

    protected void close() {
	_env.sync();

	_db.close();
	_env.close();
    }

    @SuppressWarnings("unchecked")
    protected boolean add(int blockId, long id) {
	DatabaseEntry keyEntry = new DatabaseEntry();
	keyBinding.objectToEntry(blockId, keyEntry);
	DatabaseEntry valueEntry = new DatabaseEntry();
	Transaction txn = _env.beginTransaction(null, null);
	OperationStatus status = _db.get(txn, keyEntry, valueEntry,
		LockMode.RMW);
	if (status == OperationStatus.SUCCESS) {
	    HashSet<Long> original = valueBinding.entryToObject(valueEntry);
	    if (!original.add(id)) {
		txn.abort();
		return false;
	    }
	    valueBinding.objectToEntry(original, valueEntry);
	    _db.put(txn, keyEntry, valueEntry);
	    txn.commit();
	    return true;
	} else if (status == OperationStatus.NOTFOUND) {
	    HashSet<Long> original = new HashSet<Long>();
	    original.add(id);
	    valueBinding.objectToEntry(original, valueEntry);
	    _db.put(txn, keyEntry, valueEntry);
	    txn.commit();
	    return true;
	} else {
	    System.out.println("add status " + status);
	}
	return false;
    }

    @SuppressWarnings("unchecked")
    protected HashSet<Long> get(int blockId) {
	DatabaseEntry keyEntry = new DatabaseEntry();
	keyBinding.objectToEntry(blockId, keyEntry);

	DatabaseEntry valueEntry = new DatabaseEntry();
	Transaction txn = _env.beginTransaction(null, null);
	OperationStatus status = _db.get(txn, keyEntry, valueEntry,
		LockMode.READ_COMMITTED);
	if (status == OperationStatus.SUCCESS) {
	    HashSet<Long> original = valueBinding.entryToObject(valueEntry);
	    return original;
	} else {
	    return null;
	}
    }
}
